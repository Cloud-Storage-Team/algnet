import os
import sys

def generate_html(directory):
    # Начинаем с создания базовой структуры HTML
    html_content = """
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Image Gallery</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin: 0;
                padding: 20px;
                background-color: #f4f4f4;
            }
            h1 {
                color: #333;
            }
            h2 {
                color: #555;
            }
            .gallery {
                display: flex;
                flex-wrap: wrap;
                justify-content: center;
                margin-top: 20px;
            }
            .gallery img {
                max-width: 200px;
                margin: 10px;
                border: 1px solid #ccc;
                border-radius: 5px;
                background-color: #fff;
                padding: 5px;
            }
        </style>
    </head>
    <body>
    """

    # Проходим по всем поддиректориям и файлам
    for root, dirs, files in os.walk(directory):
        # Список для хранения изображений в текущей директории
        images = [f for f in files if f.lower().endswith('.png')]

        if images:
            # Добавляем заголовок для текущей директории
            relative_path = os.path.relpath(root, directory)
            html_content += f"<h2>{relative_path}</h2><div class='gallery'>"

            for image in images:
                image_path = os.path.join(relative_path, image)
                html_content += f"<img src='{image_path}' alt='{image}'>"

            html_content += "</div>"

    # Закрываем HTML-теги
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

    output_file = os.path.join(input_directory, "gallery.html")

    html_content = generate_html(input_directory)
    save_html_file(html_content, output_file)

    print(f"HTML file successfully created and put to {output_file}.")
