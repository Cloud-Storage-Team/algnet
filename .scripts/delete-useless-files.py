import os
import subprocess
import shutil

def get_git_branches():
    """Gets a list of branch names in remote repository."""
    try:
        subprocess.run(['git', 'fetch', '--prune'], check=True)

        remote_branches = subprocess.check_output(['git', 'branch', '-r', '--format=%(refname:short)'], stderr=subprocess.STDOUT)
        remote_branches = remote_branches.decode('utf-8').splitlines()
        remote_branches = [branch.strip().replace('origin/', '') for branch in remote_branches]

        return set(remote_branches)
    except subprocess.CalledProcessError as e:
        print("Error while retrieving branches:", e.output.decode())
        return set()

def clean_directory(branches):
    """Removes files and directories except those that match branch names."""
    current_dir = os.getcwd()

    for item in os.listdir(current_dir):
        if item.startswith('.'):
            continue

        if item not in branches:
            item_path = os.path.join(current_dir, item)
            try:
                if os.path.isdir(item_path):
                    shutil.rmtree(item_path)
                    print(f"Removed directory: {item_path}")
                else:
                    os.remove(item_path)
                    print(f"Removed file: {item_path}")
            except Exception as e:
                print(f"Cannot delete {item_path}: {e}")

if __name__ == "__main__":
    branches = get_git_branches()
    print("Existing branches:", branches)
    clean_directory(branches)
