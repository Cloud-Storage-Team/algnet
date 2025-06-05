import os
import subprocess
import shutil

def get_git_branches():
    """Получает список названий веток в текущем Git-репозитории."""
    try:
        branches = subprocess.check_output(
            ['git', 'branch', '--format=%(refname:short)'],
            stderr=subprocess.STDOUT
        ).decode('utf-8').splitlines()
        return set(branches)
    except subprocess.CalledProcessError as e:
        print("Ошибка при получении веток:", e.output.decode())
        return set()

def clean_directory(branches):
    """Удаляет файлы и директории, кроме тех, что соответствуют названиям веток."""
    current_dir = os.getcwd()

    for item in os.listdir(current_dir):
        if item.startswith('.'):
            continue

        if item not in branches:
            item_path = os.path.join(current_dir, item)
            try:
                if os.path.isdir(item_path):
                    shutil.rmtree(item_path)
                    print(f"Remove directory: {item_path}")
                else:
                    os.remove(item_path)
                    print(f"Remove file: {item_path}")
            except Exception as e:
                print(f"Can not delete {item_path}: {e}")

if __name__ == "__main__":
    branches = get_git_branches()
    print("Existing branches:", branches)
    clean_directory(branches)
