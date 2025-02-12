import os
import os_commands

BUILD_DIR_NAME = "build"

def main():
    os_commands.go_to_project_root()
    os.chdir(BUILD_DIR_NAME)
    os_commands.try_os("cmake --build .")
    os.chdir("Debug")
    os_commands.clear_console()
    os_commands.try_os("simulator.exe")

if __name__ == "__main__":
    main()
