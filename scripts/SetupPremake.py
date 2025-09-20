import Utils
import os
import sys
import platform

class PremakeSetup:

    SUPPORTED_VERSIONS = ["5.0.0-beta7"]
    SUPPORTED_PLATFORMS = {
        "windows": "windows",
        "darwin": "macosx"
    }
    PREMAKE_DIRECTORY = "./premake"
    PREMAKE_URL_TEMPLATE = "https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-{premake_platform}.{extension}" 
    PREMAKE_LICENSE_URL = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"

    @classmethod
    def setup(cls):
        premake_extension = ".exe" if platform.system().lower() == "windows" else ""
        if not os.path.exists(f"{cls.PREMAKE_DIRECTORY}/premake5{premake_extension}"):

            premake_platform = ""
            try:
                premake_platform = cls.SUPPORTED_PLATFORMS[platform.system().lower()]
            except KeyError:
                print("Unsupported platform")
                sys.exit()

            chosen_premake_version = ""
            while True:
                try:
                    print("Select a premake version:")
                    version_index = 0
                    for supported_premake_version in cls.SUPPORTED_VERSIONS:
                        sys.stdout.write("\t{index}. {version}\n"
                                         .format(index = version_index, version = supported_premake_version))
                        version_index += 1
                    
                    chosen_premake_version_index = int(input())
                    chosen_premake_version = cls.SUPPORTED_VERSIONS[chosen_premake_version_index]
                    break
                except (ValueError, IndexError):
                    version_index = 0
                    print("Error while selecting version, please try again")
                except KeyboardInterrupt:
                    sys.exit("Exiting...")

            cls.__install_premake(chosen_premake_version, premake_platform)
        else:
            print("Premake already installed!")

    @classmethod
    def __install_premake(cls, version, platform):
        
        print(f"Starting download for premake {version} for {platform}.")
        os.makedirs(cls.PREMAKE_DIRECTORY)
        premake_extension = "zip" if platform == "windows" else "tar.gz"
        premake_url = cls.PREMAKE_URL_TEMPLATE.format(premake_version = version, premake_platform = platform, extension = premake_extension)
        premake_path = f"{cls.PREMAKE_DIRECTORY}/premake.{premake_extension}"
        Utils.download_file(premake_url, premake_path)
        Utils.unzip_file(premake_path)
        print(f"Premake {version} has been downloaded!")

        premake_license_path = f"{cls.PREMAKE_DIRECTORY}/LICENSE.txt"
        Utils.download_file(cls.PREMAKE_LICENSE_URL, premake_license_path)
        print("Premake License has been downloaded!")
        print("Premake setup complete.")

