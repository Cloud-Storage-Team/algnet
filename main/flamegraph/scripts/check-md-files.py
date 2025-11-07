import argparse
import os
import sys
import subprocess

SCRIPT_DIR_PATH = os.path.dirname(__file__)
CHECK_SCRIPT = os.path.join(SCRIPT_DIR_PATH, "check-md-links.py")

# Return true or false
def main(dir : str, ignore_prefix : str):
    incorrect_files = []
    for root, dirs, files in os.walk(dir):
        relpath = os.path.relpath(root, dir)
        for file in files:
            if not file.endswith('.md'):
                continue
            if relpath.startswith(ignore_prefix):
                continue

            file = os.path.join(relpath, file)

            print(f"Start to check file {file}:")
            
            run_args = [
                "python3",
                CHECK_SCRIPT,
                "-f",
                file
            ]

            print(f"args: {run_args}")
            result = subprocess.run(run_args, capture_output=True, text=True, check=False)
            print(result.stdout)
            if result.returncode != 0:
                incorrect_files.append((file, result.stderr))
    if len(incorrect_files) == 0:
        print("All mardown files are correct!")
        return True
    print("Check failed for those files:", file=sys.stderr)
    for file, stderr in incorrect_files:
        print(f"File: {file}", file=sys.stderr)
        print("stderr:", file=sys.stderr)
        print(stderr, file=sys.stderr)
        print("==================================", file=sys.stderr)
    return False

if __name__== "__main__":
    parser = argparse.ArgumentParser(
        description="Check are all mardown files in given directory and its subdirectories are correct"
    )
    parser.add_argument("-d", "--directory", help="Path to the directory to be checked", required=True)
    parser.add_argument("-i", "--ignore_prefix", help="Directories prefix", required=True)
    args = parser.parse_args()
    if not main(args.directory, args.ignore_prefix):
        sys.exit(-1)