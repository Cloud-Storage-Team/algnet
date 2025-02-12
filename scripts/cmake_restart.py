import os
import os_commands
import build_and_run

dirpath = os.path.dirname(__file__)
project_path = os.path.dirname(dirpath)
build_dir_path = os.path.join(project_path, "build")

os.chdir(project_path)
os_commands.try_os("wsl rm -rf build")
os.mkdir("build")
os.chdir("build")

os_commands.try_os(f"cmake .. -G \"Visual Studio 17 2022\"")
build_and_run.main()