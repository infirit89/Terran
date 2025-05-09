import requests
import os
import sys
import zipfile

TerranRoot = "."
TerranEnginePath = f"{TerranRoot}/TerranEngine"
TerranEngineVendorPath = f"{TerranEnginePath}/vendor"
TerranEditorPath = f"{TerranRoot}/TerranEditor"

def DownloadFile(url, filepath):
    print("Downloading", url)

    filepath = os.path.abspath(filepath)

    with open(filepath, "wb") as f:
        headers = {'User-Agent': "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/83.0.4103.97 Safari/537.36"}
        response = requests.get(url, headers=headers, stream = True)

        totalFileSize = response.headers.get("Content-Length")
        
        if totalFileSize is None:
            f.write(response.content)
            return

        totalFileSize = int(totalFileSize)
        
        donwloaded = 0
        for chunk in response.iter_content(chunk_size=int(max(totalFileSize / 1000, 1024))):
            donwloaded += len(chunk)
            f.write(chunk)

            percentage = (donwloaded / totalFileSize) * 100 if donwloaded < totalFileSize else 100

            sys.stdout.write("\r{:.2f}%".format(percentage))
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

            sys.stdout.write("\rExtracting: {} {:.2f}%".format(zipFileName, percentage))
            sys.stdout.flush()

        sys.stdout.write('\n')

    if(deleteZip):
        os.remove(zipPath)
