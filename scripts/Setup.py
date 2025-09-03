import sys
import subprocess
import os

packages = [ "requests", "numpy", "GitPython" ]

def InstallPackages():
    for package in packages:
        sys.stdout.write("\rInstalling package: {}\n".format(package))
        subprocess.check_call([sys.executable, "-m", "pip", "install", package])
        sys.stdout.write('\n')

def UpdateSubmodules():
    sys.stdout.write("\rUpdating submodules")
    subprocess.check_call(["git", "submodule", "update", "--init", "--recursive"])
    sys.stdout.write('\n')

if __name__ == "__main__":
    UpdateSubmodules()
    os.chdir("../")

    # TODO: install dotnet8 if not installed 
    InstallPackages()
    from SetupPremake import PremakeSetup
    from SetupShaderc import ShadercSetup
    PremakeSetup.Setup()
    ShadercSetup.Setup()
