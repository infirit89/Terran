import Utils
import os

class PremakeSetup:

    premakeVersion = "5.0.0-beta2"
    premakeDir = "../premake"
    premakeURL = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip" 
    premakeLicenseURL = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"

    @classmethod
    def Setup(self):
        if not os.path.exists(f"{self.premakeDir}/premake5.exe"):
            self.InstallPremake()
        else:
            print("Correct version of premake already exists!")

    @classmethod
    def InstallPremake(self):
        
        os.makedirs(self.premakeDir)
        premakePath = f"{self.premakeDir}/premake.zip";
        print("Downloading {0:s} to {1:s}".format(self.premakeURL, premakePath))
        Utils.DownloadFile(self.premakeURL, premakePath)
        Utils.UnzipFile(f"{self.premakeDir}/premake.zip")
        print(f"Premake {self.premakeVersion} has been downloaded!")

        premakeLicensePath = f"{self.premakeDir}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(self.premakeLicenseURL, premakeLicensePath))
        Utils.DownloadFile(self.premakeLicenseURL, premakeLicensePath)
        print("Premake License has been downloaded!")


