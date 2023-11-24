import Utils
import os
import shutil
import sys

class ShadercSetup:

    shadercInstallDir = f"{Utils.TerranEngineVendorPath}/shaderc"

    shadercURL = {
        "win32": "https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/windows/continuous_release_2017/438/20230810-123700/install.zip",
        "linux": "https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/linux/continuous_clang_release/439/20231013-080237/install.tgz"
    }
    shadercFileName = {
        "win32": "shaderc.zip",
        "linux": "shaderc.tgz"
    }
    
    shadercLicenseURL = f"https://raw.githubusercontent.com/google/shaderc/main/LICENSE"

    @classmethod
    def Setup(self):
        if not os.path.exists(self.shadercInstallDir):
            self.InstallShaderc()
        else:
            print("Shaderc already installed!")

    @classmethod
    def InstallShaderc(self):

        os.makedirs(self.shadercInstallDir)
        shadercZipPath = f"{self.shadercInstallDir}/{self.shadercFileName[sys.platform]}"
        Utils.DownloadFile(self.shadercURL[sys.platform], shadercZipPath)

        if sys.platform == 'win32':
            Utils.UnzipFile(shadercZipPath)
        elif sys.platform == 'linux':
            Utils.UnzipTar(shadercZipPath)
            
        self.MoveFromTempDirectory()
        
        print("Shaderc has been downloaded!")

        shadercLicensePath = f"{self.shadercInstallDir}/LICENSE.txt"
        Utils.DownloadFile(self.shadercLicenseURL, shadercLicensePath)
        print("Shaderc license has been downloaded")

    @classmethod
    def MoveFromTempDirectory(self):
        shadercTempPath = os.path.abspath(f"{self.shadercInstallDir}/install")
        totalEntryCount = len(os.listdir(shadercTempPath))
        moved = 0
        
        for subdir in os.scandir(shadercTempPath):
            if subdir.is_dir():
                shutil.move(f"{shadercTempPath}/{subdir.name}", self.shadercInstallDir)
                sys.stdout.write("\rMoved: {}/{}".format(moved, totalEntryCount))
                sys.stdout.flush()
                moved += 1
        sys.stdout.write("\n")
        
        shutil.rmtree(shadercTempPath)

