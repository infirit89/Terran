import Utils

premakeDir = "../premake"
premakeURL = "https://github.com/premake/premake-core/releases/download/v5.0.0-beta1/premake-5.0.0-beta1-windows.zip" 
premakeLicenseURL = "https://github.com/premake/premake-core/blob/master/LICENSE.txt"

if __name__ == "__main__":
    Utils.DownloadFile(premakeURL, f"{premakeDir}/premake.zip")
    Utils.DownloadFile(premakeLicenseURL, f"{premakeDir}/LICENSE.txt")
    Utils.UnzipFile(f"{premakeDir}/premake.zip")

