#!/usr/bin/env python3
import os
import sys
import re
import shutil
import argparse
from typing import List, Dict, Tuple
from yattag import Doc

# relative path from current file
TEMPLATES_DIR = "../.templates"

def natural_key(s: str) -> List[object]:
    """
    Split a string into numeric and text parts for “natural” sorting
    (so that file2 comes before file10).
    """
    return [
        int(chunk) if chunk.isdigit() else chunk.lower()
        for chunk in re.split(r'(\d+)', s)
        if chunk
    ]

def generate_html(source_dir: str, css_path) -> str:
    """
    Walk through all subdirectories of source_dir,
    find those containing index.html, group them by their first
    path component, sort groups and links, and return the HTML text.
    """
    groups: Dict[str, List[Tuple[str, str]]] = {}
    for root, dirs, files in os.walk(source_dir):
        # skip the root directory itself and any folder without index.html
        if root == source_dir or 'index.html' not in files:
            continue

        rel_path = os.path.relpath(root, source_dir).replace(os.path.sep, '/')
        parts = rel_path.split('/')
        group = parts[0]
        # if more than one component, join the rest with ' > '
        sublabel = ' > '.join(parts[1:]) if len(parts) > 1 else parts[0]
        href = f"{rel_path}/index.html"
        groups.setdefault(group, []).append((sublabel, href))

    # sort groups: 'main' first, then alphabetically
    sorted_groups: List[str] = sorted(
        groups.keys(),
        key=lambda g: (g.lower() != 'main', g.lower())
    )

    # within each group, sort links in reverse “natural” order
    for grp in sorted_groups:
        groups[grp].sort(
            key=lambda item: natural_key(item[0]),
            reverse=True
        )

    # build the HTML document using yattag
    doc, tag, text = Doc().tagtext()
    doc.asis('<!DOCTYPE html>')
    with tag('html', lang='en'):
        with tag('head'):
            doc.stag('meta', charset='UTF-8')
            doc.stag('meta', **{
                'name': 'viewport',
                'content': 'width=device-width, initial-scale=1'
            })
            with tag('title'):
                text('Metrics')
            # link to the external CSS file
            doc.stag('link', rel='stylesheet', href=css_path)

        with tag('body'):
            with tag('h1'):
                text('Metrics')
            with tag('div', klass='container'):
                for grp in sorted_groups:
                    with tag('div', klass='group'):
                        with tag('div', klass='group-title'):
                            text(grp)
                        for label, href in groups[grp]:
                            with tag('a', href=href, klass='button'):
                                text(label)

    return doc.getvalue()

def save_file(content: str, output_path: str):
    """
    Write content to output_path, creating parent directories if needed.
    """
    out_dir = os.path.dirname(output_path)
    if out_dir and not os.path.exists(out_dir):
        os.makedirs(out_dir, exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(content)

def copy_css(template_css_path: str, target_dir: str):
    """
    Copy the CSS template file to the target directory.
    """
    if not os.path.exists(target_dir):
        os.makedirs(target_dir, exist_ok=True)
    dest: str = os.path.join(target_dir, os.path.basename(template_css_path))
    shutil.copy(template_css_path, dest)

def parse_args() -> argparse.Namespace:
    """
    Parse command line arguments for source and output directories.
    """
    parser = argparse.ArgumentParser(
        description="Generate an HTML index of metric pages."
    )
    parser.add_argument(
        '--source-dir', '-s',
        default='',
        help="Path to the directory containing metric subfolders (default: current directory)."
    )
    parser.add_argument(
        '--output-dir', '-o',
        default='',
        help="Directory where HTML and CSS will be saved (default: current directory)."
    )
    return parser.parse_args()

def main():
    args = parse_args()

    # use current working directory if none provided
    source_dir = args.source_dir or os.getcwd()
    output_dir = args.output_dir or os.getcwd()

    # change working dir to the script's dir to locate styles.css
    script_dir = os.path.dirname(os.path.abspath(__file__))
    os.chdir(script_dir)

    # ensure the CSS template exists next to the script
    template_css_path = os.path.join(script_dir, TEMPLATES_DIR, 'styles.css')
    if not os.path.isfile(template_css_path):
        print(f"Error: CSS template not found: {template_css_path}", file=sys.stderr)
        sys.exit(1)

    # 1) copy the CSS file into the output directory
    copy_css(template_css_path, output_dir)

    css_dir = os.path.basename(template_css_path)
    print(f"CSS copied to {os.path.join(output_dir, css_dir)}")

    # 2) generate the HTML and save it to output_dir/index.html
    html: str = generate_html(source_dir, css_dir)
    output_html: str = os.path.join(output_dir, 'index.html')
    save_file(html, output_html)
    print(f"HTML saved to {output_html}")

if __name__ == '__main__':
    main()
