import Utils
import os
import shutil
import sys

class ShadercSetup:

    SHADERC_INSTALL_DIRECTORY = f"{Utils.TERRAN_ENGINE_VENDOR_PATH}/shaderc"
    SHADERC_URL = f"https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/windows/continuous_release_2017/438/20230810-123700/install.zip"
    SHADERC_LICENSE_URL = f"https://raw.githubusercontent.com/google/shaderc/main/LICENSE"

    @classmethod
    def setup(cls):
        if not os.path.exists(cls.SHADERC_INSTALL_DIRECTORY):
            cls.__install_shaderc()
        else:
            print("Shaderc already installed!")

    @classmethod
    def __install_shaderc(cls):

        os.makedirs(cls.SHADERC_INSTALL_DIRECTORY)
        shaderc_zip_path = f"{cls.SHADERC_INSTALL_DIRECTORY}/shaderc.zip"
        Utils.download_file(cls.SHADERC_URL, shaderc_zip_path)
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

