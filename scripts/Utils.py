import requests
import os
import sys
import zipfile
import tarfile
import platform

TERRAN_ROOT = "."
TERRAN_ENGINE_PATH = f"{TERRAN_ROOT}/TerranEngine"
TERRAN_ENGINE_VENDOR_PATH = f"{TERRAN_ENGINE_PATH}/vendor"
TERRAN_EDITOR_PATH = f"{TERRAN_ROOT}/TerranEditor"

def download_file(url, filepath):
    filepath = os.path.abspath(filepath)

    with open(filepath, "wb") as f:
        response = requests.get(url, stream = True)

        total_file_size = response.headers.get("Content-Length")
        
        if total_file_size is None:
            f.write(response.content)
            return

        total_file_size = int(total_file_size)
        
        donwloaded = 0
        for chunk in response.iter_content(chunk_size=int(max(total_file_size / 1000, 1024 * 1024))):
            donwloaded += len(chunk)
            f.write(chunk)

            percentage = (donwloaded / total_file_size) * 100 if donwloaded < total_file_size else 100

            sys.stdout.write("\rDownloading {:s} {:.2f}%".format(os.path.basename(url), percentage))
            sys.stdout.flush()
        sys.stdout.write('\n')

def unzip_file(zip_path, delete_zip : bool = True):
    if( platform.system().lower() == "windows"):
        __unzip_file_windows(zip_path, delete_zip)
    else:
        __unzip_file_unix(zip_path, delete_zip)

def __unzip_file_windows(zip_path, delete_zip : bool = True):
    zip_path = os.path.abspath(zip_path)
    zip_file_location = os.path.dirname(zip_path)
    zip_file_name = os.path.basename(zip_path)

    with zipfile.ZipFile(zip_path, 'r') as z:
        total_zip_size = 0
        for zipped_file_name in z.namelist():
            total_zip_size += z.getinfo(zipped_file_name).file_size
        
        extracted = 0
        for zipped_file_name in z.namelist():
            z.extract(zipped_file_name, zip_file_location)
            extracted += z.getinfo(zipped_file_name).file_size

            percentage = (extracted / total_zip_size) * 100

            sys.stdout.write("\rExtracting {:s} {:.2f}%".format(zip_file_name, percentage))
            sys.stdout.flush()

        sys.stdout.write('\n')

    if(delete_zip):
        os.remove(zip_path)

def __unzip_file_unix(zip_path, delete_zip : bool = True):
    zip_path = os.path.abspath(zip_path)
    zip_file_location = os.path.dirname(zip_path)
    zip_file_name = os.path.basename(zip_path)

    with tarfile.open(zip_path, 'r') as z:
        total_zip_size = 0
        tar_member_names = z.getnames()

        for zipped_file_name in tar_member_names:
            total_zip_size += z.getmember(zipped_file_name).size
        
        extracted = 0
        for zipped_file_name in tar_member_names:
            z.extract(zipped_file_name, zip_file_location)
            extracted += z.getmember(zipped_file_name).size

            percentage = (extracted / total_zip_size) * 100

            sys.stdout.write("\rExtracting {:s} {:.2f}%".format(zip_file_name, percentage))
            sys.stdout.flush()

        sys.stdout.write('\n')

    if(delete_zip):
        os.remove(zip_path)

