import time
import requests
import argparse


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Calculate deploy destination directory."
    )
    parser.add_argument(
        "-u",
        "--url",
        help="GitHub Pages URL",
        required=True,
    )
    return parser.parse_args()


def main():
    args = parse_arguments()
    while True:
        try:
            response = requests.head(args.url)
            if response.status_code == 200:
                print(f"Artifacts are available at {args.url}")
                break
            else:
                print("Waiting for page to be available...")
                time.sleep(1)
        except requests.RequestException:
            print("Waiting for page to be available...")
            time.sleep(1)


if __name__ == "__main__":
    main()
