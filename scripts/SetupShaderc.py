import Utils
import os
import shutil
import sys
import platform

class ShadercSetup:

    SUPPORTED_PLATFORMS = {
        "windows": "windows",
        "darwin": "macos"
    }
    SHADERC_INSTALL_DIRECTORY = f"{Utils.TERRAN_ENGINE_VENDOR_PATH}/shaderc"
    SHADERC_URL_TEMPLATE = "https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/{shaderc_platform}/{shaderc_configuration}/{shaderc_version}/install.{shaderc_extension}"
    SHADERC_LICENSE_URL = f"https://raw.githubusercontent.com/google/shaderc/main/LICENSE"

    @classmethod
    def setup(cls):
        if not os.path.exists(cls.SHADERC_INSTALL_DIRECTORY):
            shaderc_platform = ""
            try:
                shaderc_platform = cls.SUPPORTED_PLATFORMS[platform.system().lower()]
            except KeyError:
                print("Unsupported platform")

            shaderc_configuration = "continuous_clang_release" if platform.system().lower() == "darwin" else "vs2022_amd64_release_continuous"

            shaderc_version = "512/20250905-100502" if platform.system().lower() == "darwin" else "22/20250905-100500"
            cls.__install_shaderc(shaderc_platform, shaderc_configuration, shaderc_version)
        else:
            print("Shaderc already installed!")

    @classmethod
    def __install_shaderc(cls, platform, configuration, version):

        os.makedirs(cls.SHADERC_INSTALL_DIRECTORY)
        shaderc_extension = "zip" if platform == "windows" else "tgz"
        shaderc_zip_path = f"{cls.SHADERC_INSTALL_DIRECTORY}/shaderc.{shaderc_extension}"
        shaderc_url = cls.SHADERC_URL_TEMPLATE.format(
            shaderc_platform = platform,
            shaderc_configuration = configuration,
            shaderc_version = version,
            shaderc_extension = shaderc_extension)

        Utils.download_file(shaderc_url, shaderc_zip_path)
        Utils.unzip_file(shaderc_zip_path)
        cls.__move_from_temporary_directory()
        
        print("Shaderc has been downloaded!")

        shaderc_license_path = f"{cls.SHADERC_INSTALL_DIRECTORY}/LICENSE.txt"
        Utils.download_file(cls.SHADERC_LICENSE_URL, shaderc_license_path)
        print("Shaderc license has been downloaded")

    @classmethod
    def __move_from_temporary_directory(cls):
        shaderc_temporary_path = os.path.abspath(f"{cls.SHADERC_INSTALL_DIRECTORY}/install")
        total_entry_count = len(os.listdir(shaderc_temporary_path))
        moved = 0
        
        for subdirectory in os.scandir(shaderc_temporary_path):
            if subdirectory.is_dir():
                shutil.move(f"{shaderc_temporary_path}/{subdirectory.name}", cls.SHADERC_INSTALL_DIRECTORY)
                sys.stdout.write("\rMoved: {}/{}".format(moved, total_entry_count))
                sys.stdout.flush()
                moved += 1
        sys.stdout.write("\n")
        
        shutil.rmtree(shaderc_temporary_path)

