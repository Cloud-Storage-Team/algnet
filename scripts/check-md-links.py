import re
import requests
import os
import argparse
import sys

# Returns true if all links are correct, false otherwise
def check_links_in_markdown(file_path : str):
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    # Change directory to given file directory for
    # correct check of relative lins
    file_dir = os.path.dirname(file_path)
    if file_dir != "":
        os.chdir(file_dir)
    # Regular expression matches markdown links
    # Part [^\[^\(]* that repeats twice
    # parses any string that do not contain [ or (
    pattern = r'\[[^\[^\(]*\]\([^\[^\(]*\)'
    links = re.findall(pattern, content)

    incorrect_links = []

    for link in links:
        print()
        link_path = link[link.index('(') + 1 :link.index(')')]
        print(f"Processing link {link_path}")
        if os.path.exists(link_path):
            print(f"{link_path} is path to a file")
            continue
        # Check path as a link
        link_url = link_path
        try:
            response = requests.head(link_url, allow_redirects=True)
            if response.status_code != 200:
                raise requests.exceptions.ConnectionError(f"Can not access to {link_url}")
            print(f"{link_url} is link to an available site")
        except requests.exceptions.RequestException:
            print(f"{link_url} is not an available site; returned code {response.status_code}")
            incorrect_links.append(link_url)
    if len(incorrect_links) == 0:
        print()
        print("All links are correct!")
        return True
    print("Found incorrect links. Each of them is neither the correct path to a file nor a link to an available site:", file=sys.stderr)
    for link in incorrect_links:
        print(f"link: {link}", file=sys.stderr)
        print("====================", file=sys.stderr)
    return False
def main():
    parser = argparse.ArgumentParser(
        description="Check that all the links in given markdown file are correct pathes to some files or the links to the available sites"
    )
    parser.add_argument("-f", "--file", help="Path to the file to be checked", required=True)
    args = parser.parse_args()
    if not(check_links_in_markdown(args.file)):
        exit(-1)

if __name__ == "__main__":
    main()