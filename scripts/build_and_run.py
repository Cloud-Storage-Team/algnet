import os
import os_commands

def main():

    dirpath = os.path.dirname(__file__)
    project_path = os.path.dirname(dirpath)
    build_dir_path = os.path.join(project_path, "build")

    os.chdir(build_dir_path)
    os_commands.try_os("cmake --build .")
    os.chdir("Debug")
    os_commands.clear_console()
    os_commands.try_os("simulator.exe")

main()