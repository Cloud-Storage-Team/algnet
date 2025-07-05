import argparse
import sys
import subprocess
import re


def parse_arguments():
    parser = argparse.ArgumentParser(
        description="Calculate deploy destination directory."
    )
    parser.add_argument(
        "--github-event-name",
        help="Type of GitHub event that triggered the workflow",
        required=True,
    )
    parser.add_argument(
        "--github-head-ref",
        help="TODO",
        required=True,
    )
    parser.add_argument(
        "--github-run-id",
        help="GitHub Workflow run ID",
        required=True,
    )
    parser.add_argument(
        "--github-ref",
        help="TODO",
        required=True,
    )
    parser.add_argument(
        "--github-env-varname",
        help="Name of variable for GitHub environment",
        required=True,
    )
    parser.add_argument(
        "--deploy-dir-varname",
        help="Name of environment variable to store deploy directory",
        required=True,
    )
    return parser.parse_args()


def main():
    args = parse_arguments()
    if args.github_event_name == "pull_request":

        subprocess_args = [
            "echo",
            f'"{args.deploy_dir_varname}={args.github_head_ref}/{args.github_run_id}"',
            ">>",
            f"${args.github_env_varname};",
        ]

        subprocess.run(
            subprocess_args,
            capture_output=True,
            check=True,
        )

        # debug
        for a in subprocess_args:
            print(a, end=" ")

    elif args.github_event_name == "push":
        branch_name = re.sub(r"^refs/heads/", "", args.github_ref)

        subprocess_args = [
            "echo",
            f'"{args.deploy_dir_varname}={branch_name}/"',
            ">>",
            f"${args.github_env_varname};",
        ]

        subprocess.run(
            subprocess_args,
            capture_output=True,
            check=True,
        )

        # debug
        for a in subprocess_args:
            print(a, end=" ")

    else:
        print(
            f"Error: Unsupported GitHub event name '{args.github_event_name}'",
            file=sys.stderr,
        )
        sys.exit(1)


if __name__ == "__main__":
    main()
