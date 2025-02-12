import os
import shutil
import os_commands
import build_and_run

def main():
    os_commands.go_to_project_root()
    if os.path.exists(build_and_run.BUILD_DIR_NAME):
        shutil.rmtree(build_and_run.BUILD_DIR_NAME)
        print("build directory deleted")
    else:
        print("build directory not found")

    os.mkdir(build_and_run.BUILD_DIR_NAME)
    os.chdir(build_and_run.BUILD_DIR_NAME)

    os_commands.try_os(f"cmake .. -G \"Visual Studio 17 2022\"")
    build_and_run.main()

if __name__ == "__main__":
    main()
