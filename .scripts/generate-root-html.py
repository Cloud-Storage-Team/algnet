import os
import sys

def generate_html(dir="."):
    html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Metrics</title>
    <style>
        body {{
            font-family: Arial, sans-serif;
            text-align: center;
            margin: 0;
            padding: 20px;
            background-color: #f4f4f4;
        }}
        h1 {{
            color: #333;
            font-size: 24px;
            margin-bottom: 20px;
        }}
        .button {{
            display: block;
            padding: 10px 20px;
            margin: 10px auto;
            font-size: 16px;
            color: white;
            background-color: #007BFF;
            border: none;
            border-radius: 5px;
            text-decoration: none;
            transition: background-color 0.3s;
            width: 200px;
        }}
        .button:hover {{
            background-color: #0056b3;
        }}
    </style>
</head>
<body>
    <h1>Metrics</h1>
    """

    buttons = []

    for root, dirs, files in os.walk(dir):
        if root == dir:
            continue
        for file in files:
            if file == 'index.html':
                file_path = os.path.relpath(os.path.join(root, file), dir)
                link_name = os.path.relpath(root, dir).replace(os.path.sep, ' > ')
                buttons.append((link_name, file_path))

    buttons.sort(key=lambda x: (x[0] != 'main', x[0].lower()))

    for link_name, file_path in buttons:
        html_content += f"<a href='{file_path}' class='button'>{link_name}</a>\n"

    html_content += """
    </body>
    </html>
    """

    return html_content

def save_html_file(html_content, output_file):
    with open(output_file, "w", encoding='utf-8') as f:
        f.write(html_content)

if __name__ == "__main__":
    output_file = sys.argv[1]

    html_content = generate_html()

    dirpath = os.path.dirname(output_file)
    if (dirpath != ''):
        os.makedirs(dirpath, exist_ok=True)
    save_html_file(html_content, output_file)

    print(f"HTML file successfully created and saved to {output_file}.")
