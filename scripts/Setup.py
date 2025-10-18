import sys
import subprocess
import os

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

if __name__ == "__main__":
    UpdateSubmodules()

    # TODO: install dotnet8 if not installed 
    InstallPackages()

    from SetupPremake import PremakeSetup
    from SetupShaderc import ShadercSetup

    PremakeSetup.setup()
    ShadercSetup.setup()
