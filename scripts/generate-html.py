import os
import sys

def generate_html(directory):
    folder_name = os.path.basename(directory)
    html_content = f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>{folder_name} - Image Gallery</title>
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
                margin-bottom: 10px;
            }}
            h2 {{
                color: #555;
                font-size: 20px;
                margin-top: 20px;
            }}
            .gallery {{
                display: flex;
                flex-wrap: wrap;
                justify-content: center;
                margin-top: 10px;
            }}
            .gallery img {{
                max-width: 300px;
                height: auto;
                margin: 10px;
                border: 1px solid #ccc;
                border-radius: 5px;
                background-color: #fff;
            }}
        </style>
    </head>
    <body>
        <h1>{folder_name}</h1>
    """

    for root, dirs, files in os.walk(directory):
        images = [f for f in files if f.lower().endswith('.png')]
        if images:
            relative_path = os.path.relpath(root, directory)
            html_content += f"<h2>{relative_path}</h2><div class='gallery'>"
            for image in images:
                image_path = os.path.join(relative_path, image)
                html_content += f"<img src='{image_path}' alt='{image}'>"
            html_content += "</div>"

    html_content += """
    </body>
    </html>
    """

    return html_content

def save_html_file(html_content, output_file):
    with open(output_file, 'w') as f:
        f.write(html_content)

if __name__ == "__main__":
    input_directory = sys.argv[1]
    folder_name = os.path.basename(input_directory)
    output_file = os.path.join(input_directory, f"{folder_name}.html")
    html_content = generate_html(input_directory)
    save_html_file(html_content, output_file)
    print(f"HTML file successfully created and put to {output_file}.")
