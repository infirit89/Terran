import Utils
import os

class PremakeSetup:

    premakeVersion = "5.0.0-beta2"
    premakeDir = "../premake"
    premakeURL = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip" 
    premakeLicenseURL = "https://github.com/premake/premake-core/blob/master/LICENSE.txt"

    @classmethod
    def Setup(self):
        if not os.path.exists(f"{self.premakeDir}/premake5.exe"):
            self.InstallPremake()
        else:
            print("Correct version of premake already exists!")

    @classmethod
    def InstallPremake(self):
        
        os.makedirs(self.premakeDir)
        Utils.DownloadFile(self.premakeURL, f"{self.premakeDir}/premake.zip")
        Utils.UnzipFile(f"{self.premakeDir}/premake.zip")
        print(f"Premake {self.premakeVersion} has been downloaded!")

        Utils.DownloadFile(self.premakeLicenseURL, f"{self.premakeDir}/LICENSE.txt")
        print("Premake License has been downloaded!")


