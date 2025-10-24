import argparse
import sys
import re
import json


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Calculate deploy destination directory."
    )
    parser.add_argument(
        "-c",
        "--context",
        help="GitHub context JSON file",
        required=True,
    )
    return parser.parse_args()


def load_github_context(context_file):
    with open(context_file, "r", encoding="utf-8") as file:
        return json.load(file)


def main():
    args = parse_arguments()

    github_ctx = load_github_context(args.context)

    if github_ctx["event_name"] == "pull_request":
        deploy_dir = f"{github_ctx["head_ref"]}/{github_ctx["run_id"]}"
    elif github_ctx["event_name"] == "push":
        branch_name = re.sub(r"^refs/heads/", "", github_ctx["ref"])
        deploy_dir = f"{branch_name}/"
    else:
        print(
            f"Error: Unsupported GitHub event name '{github_ctx["event_name"]}'",
            file=sys.stderr,
        )
        sys.exit(1)

    print(deploy_dir)


if __name__ == "__main__":
    main()
