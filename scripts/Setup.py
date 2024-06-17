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

    # maybe for future????
    # from git import Repo
    # from GitUpdateProgress import GitUpdateProgress

    # repo = Repo("./")
    # for submodule in repo.submodules:
    #     print(submodule.name)
    #     submodule.update(recursive=True, init=True, progress=GitUpdateProgress())

    # TODO: install dotnet8 if not installed 
    InstallPackages()
    from SetupPremake import PremakeSetup
    # from SetupMono import MonoSetup
    from SetupShaderc import ShadercSetup
    PremakeSetup.Setup()
    # MonoSetup.Setup()
    ShadercSetup.Setup()
