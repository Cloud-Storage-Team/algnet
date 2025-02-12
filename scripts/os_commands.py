import os
import platform

def try_os(command : str):
    res = os.system(command)
    if (res != 0):
        print(f"Command `{command}` failed with code {res}")
        exit(0)

def clear_console():
    if platform.system() == "Windows":
        try_os('cls')
    else:
        try_os('clear')