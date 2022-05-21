from re import sub
from shutil import copy

from numpy import equal
import Utils
from distutils.dir_util import copy_tree
import os
import glob
import sys

class MonoSetup:

    monoRoot = Utils.GetMonoRootDir()
    targetMonoLibDir = f"{Utils.TerranEngineVendorPath}/mono/lib/4.5"
    targetMonoFacadesLibDir = f"{targetMonoLibDir}/Facades"
    targetMonoEtcDir = f"{Utils.TerranEngineVendorPath}/mono/etc"
    targetMonoBinDir = f"{Utils.TerranEngineVendorPath}/mono/bin"
    targetMonoIncludeDir = f"{Utils.TerranEngineVendorPath}/mono/include/mono"

    @classmethod
    def Setup(self):
        self.CopyLibs()
        self.CopyEtcFiles()
        self.CopyLibraries()
        self.CopyIncludes()

    @classmethod
    def CopyLibs(self):
        monoLibPath = f"{self.monoRoot}/lib/mono/4.5" 
        
        libsExist = 0

        if not os.path.exists(self.targetMonoLibDir):
            os.makedirs(self.targetMonoLibDir)
        else:
            libsExist |= 1

        if not os.path.exists(self.targetMonoFacadesLibDir):
            os.makedirs(self.targetMonoFacadesLibDir)
        else:
            libsExist |= 2

        # TODO: bad libs exist check; make better (it check only if the directories exist)
        if libsExist >= 3: 
            return

        assemblyList = glob.glob(f"{monoLibPath}/*.dll")
        assemblyFacadesList = glob.glob(f"{monoLibPath}/Facades/*.dll")

        copied = 0
        totalAssemblyCount = len(assemblyList) + len(assemblyFacadesList)

        for assembly in assemblyList:
            copy(assembly, self.targetMonoLibDir)
            copied += 1

            percentage = (copied / totalAssemblyCount) * 100

            sys.stdout.write("\rCopying mono libs {:.2f}%".format(percentage))
            sys.stdout.flush()
        
        for assembly in assemblyFacadesList:
            copy(assembly, self.targetMonoFacadesLibDir)
            copied += 1

            percentage = (copied / totalAssemblyCount) * 100

            sys.stdout.write("\rCopying mono framework libraries {:.2f}%".format(percentage))
            sys.stdout.flush()
        

        sys.stdout.write('\n')
        print("Copied all mono framework libraries")
        
    @classmethod
    def CopyEtcFiles(self):
        monoEtcPath = f"{self.monoRoot}/etc/mono"

        totalFileCount = 0
        copiedFiles = 0

        fileDict = dict()

        # create etc subdirectories in the engine's vendor folder
        entryExistCount = 0
        dirList = os.listdir(monoEtcPath)

        for subdir in dirList:
            dirPath = os.path.join(monoEtcPath, subdir)
            targetDirPath = os.path.join(self.targetMonoEtcDir, subdir)

            if os.path.exists(targetDirPath):
                entryExistCount += 1
                continue
            
            if not os.path.isdir(dirPath):
                continue

            os.makedirs(targetDirPath)
            
            fileList = glob.glob(f"{dirPath}/*.*")
            totalFileCount += len(fileList)
            fileDict[subdir] = fileList

        if entryExistCount == len(dirList):
            return

        etcFileList = glob.glob(f"{monoEtcPath}/*")
        # a fucking normal for loop imitation because python can eat my ass
        i = 0
        while i < len(etcFileList):
            if not os.path.isfile(etcFileList[i]):
                etcFileList.pop(i)
                i -= 1

            i += 1

        totalFileCount += len(etcFileList)
        fileDict["etc"] = etcFileList

        for name, fList in fileDict.items():
            for entry in fList:
                if not os.path.isfile(entry):
                    continue
                
                targetPath = f"{self.targetMonoEtcDir}/{name}" if name != "etc" else self.targetMonoEtcDir

                copy(entry, targetPath)

                copiedFiles += 1

                percentage = (copiedFiles / totalFileCount) * 100

                sys.stdout.write("\rCopying etc files {:.2f}".format(percentage))
                sys.stdout.flush()

        sys.stdout.write('\n')

        print("Copied all etc files!")

    @classmethod
    def CopyLibraries(self):
        # TODO: copy the pdb files
        
        monoLibraries = [
            f"{self.monoRoot}/lib/mono-2.0-sgen.lib",
            f"{self.monoRoot}/bin/mono-2.0-sgen.dll"
        ]

        totalFileCount = len(monoLibraries)
        copiedCount = 0

        if not os.path.exists(self.targetMonoBinDir):
            os.makedirs(self.targetMonoBinDir)

        for lib in monoLibraries:
            if not os.path.exists(f"{self.targetMonoBinDir}/{os.path.basename(lib)}"):
                copy(lib, self.targetMonoBinDir)
            
            copiedCount += 1

            percentage = (copiedCount / totalFileCount) * 100

            sys.stdout.write("\rCopying mono libraries {:.2f}%".format(percentage))
            sys.stdout.flush()

        sys.stdout.write('\n')

        print("Copied all mono libraries!")

    @classmethod
    def CopyIncludes(self):
        monoIncludePath = f"{self.monoRoot}/include/mono-2.0/mono"
        totalFileCount = 0
        copiedFiles = 0

        fileDict = dict()

        # create include subdirectories in the engine's vendor folder
        entryExistCount = 0
        dirList = os.listdir(monoIncludePath)

        for subdir in dirList:
            dirPath = os.path.join(monoIncludePath, subdir)
            targetDirPath = os.path.join(self.targetMonoIncludeDir, subdir)

            if os.path.exists(targetDirPath):
                entryExistCount += 1
                continue
            
            if not os.path.isdir(dirPath):
                continue

            os.makedirs(targetDirPath)
            
            fileList = glob.glob(f"{dirPath}/*.*")
            totalFileCount += len(fileList)
            fileDict[subdir] = fileList

        if entryExistCount == len(dirList):
            return

        for name, fList in fileDict.items():
            for entry in fList:
                copy(entry, f"{self.targetMonoIncludeDir}/{name}")

                copiedFiles += 1

                percentage = (copiedFiles / totalFileCount) * 100

                sys.stdout.write("\rCopying includes {:.2f}".format(percentage))
                sys.stdout.flush()

        sys.stdout.write('\n')

        print("Copied all includes!")
