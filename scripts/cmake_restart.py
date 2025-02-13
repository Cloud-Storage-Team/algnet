import os
import shutil

BUILD_DIR_NAME = "build"

def go_to_project_root():
    dirpath = os.path.dirname(__file__)
    project_path = os.path.dirname(dirpath)
    os.chdir(project_path)


def try_os(command : str):
    res = os.system(command)
    if (res != 0):
        print(f"Command `{command}` failed with code {res}")
        exit(0)

def main():
    go_to_project_root()
    if os.path.exists(BUILD_DIR_NAME):
        shutil.rmtree(BUILD_DIR_NAME)
        print("build directory deleted")
    else:
        print("build directory not found")

    os.mkdir(BUILD_DIR_NAME)
    os.chdir(BUILD_DIR_NAME)

    try_os(f"cmake .. -G \"Unix Makefiles\"")

if __name__ == "__main__":
    main()
