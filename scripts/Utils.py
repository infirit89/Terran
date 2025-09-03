import requests
import os
import sys
import zipfile

TerranRoot = "."
TerranEnginePath = f"{TerranRoot}/TerranEngine"
TerranEngineVendorPath = f"{TerranEnginePath}/vendor"
TerranEditorPath = f"{TerranRoot}/TerranEditor"

def DownloadFile(url, filepath):
    filepath = os.path.abspath(filepath)

    with open(filepath, "wb") as f:
        response = requests.get(url, stream = True)

        totalFileSize = response.headers.get("Content-Length")
        
        if totalFileSize is None:
            f.write(response.content)
            return

        totalFileSize = int(totalFileSize)
        
        donwloaded = 0
        for chunk in response.iter_content(chunk_size=int(max(totalFileSize / 1000, 1024 * 1024))):
            donwloaded += len(chunk)
            f.write(chunk)

            percentage = (donwloaded / totalFileSize) * 100 if donwloaded < totalFileSize else 100

            sys.stdout.write("\rDownloading {:s} {:.2f}%".format(os.path.basename(url), percentage))
            sys.stdout.flush()
        sys.stdout.write('\n')


def UnzipFile(zipPath, deleteZip : bool = True):
    zipPath = os.path.abspath(zipPath)
    zipFileLocation = os.path.dirname(zipPath)
    zipFileName = os.path.basename(zipPath)

    with zipfile.ZipFile(zipPath, 'r') as z:
        totalZipSize = 0
        for zippedFileName in z.namelist():
            totalZipSize += z.getinfo(zippedFileName).file_size
        
        extracted = 0
        for zippedFileName in z.namelist():
            z.extract(zippedFileName, zipFileLocation)
            extracted += z.getinfo(zippedFileName).file_size

            percentage = (extracted / totalZipSize) * 100

            sys.stdout.write("\rExtracting {:s} {:.2f}%".format(zipFileName, percentage))
            sys.stdout.flush()

        sys.stdout.write('\n')

    if(deleteZip):
        os.remove(zipPath)

