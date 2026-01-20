import sys
import subprocess
import argparse

packages = [ "requests", "numpy" ]

def InstallPackages():
    for package in packages:
        sys.stdout.write("Installing package: {}\r\n".format(package))
        subprocess.check_call([sys.executable, "-m", "pip", "install", package])
        sys.stdout.write('\r\n')

def UpdateSubmodules():
    sys.stdout.write("Updating submodules\r\n")
    subprocess.check_call(["git", "submodule", "update", "--init", "--recursive"])
    sys.stdout.write('\r\n')

def parse_args():
    parser = argparse.ArgumentParser(description="Project setup script")

    parser.add_argument(
        "--premake-version",
        help="Premake version to install (e.g. 5.0.0-beta7)"
    )

    return parser.parse_args()

if __name__ == "__main__":
    
    UpdateSubmodules()

    # TODO: install dotnet8 if not installed 
    InstallPackages()

    from SetupPremake import PremakeSetup
    from SetupShaderc import ShadercSetup

    args = parse_args()

    PremakeSetup.setup(premake_version=args.premake_version)
    ShadercSetup.setup()
