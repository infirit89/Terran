import Utils
import os
import sys
class PremakeSetup:

    premakeVersion = "5.0.0-beta2"
    premakeDir = "./premake"
    premakeURL = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/"
    premakeLicenseURL = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeFileName = f"premake-{premakeVersion}"

    @classmethod
    def Setup(self):
        print("Setting up premake:")
        if sys.platform == "win32":
            self.premakeFileName += "-windows.zip"
        elif sys.platform == "linux":
            self.premakeFileName += "-linux.tar.gz"

        self.premakeURL += self.premakeFileName
        if not self.__IsPremakeInstalled():
            self.__InstallPremake()
        else:
            print("Correct version of premake already exists!")

    @classmethod
    def __IsPremakeInstalled(self):
        return os.path.exists(f"{self.premakeDir}/premake5")

    @classmethod
    def __InstallPremake(self):
        if not os.path.exists(self.premakeDir):
            os.makedirs(self.premakeDir)

        premakePath = f"{self.premakeDir}/{self.premakeFileName}"
        Utils.DownloadFile(self.premakeURL, premakePath)
        
        if sys.platform == 'win32':
            Utils.UnzipFile(premakePath)
        elif sys.platform == 'linux':
            Utils.UnzipTar(premakePath)

        print(f"Premake {self.premakeVersion} has been installed!")

        premakeLicensePath = f"{self.premakeDir}/LICENSE.txt"
        Utils.DownloadFile(self.premakeLicenseURL, premakeLicensePath)
        print("Premake License has been downloaded!")
        print("Premake setup complete")
