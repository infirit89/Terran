from abc import abstractmethod

import os
import sys

class Registry:
    @abstractmethod
    def QueryValue(self, subkey: str, valueName: str):
        pass

class WindowsRegistry(Registry):
    def __OpenKey(self, key: int, subkey: str):
        import winreg
        try:
            return winreg.OpenKey(key, subkey)
        except:
            sys.stderr.write("Couldn't open the specified key!\n")
            return None
    
    def QueryValue(self, subkey: str, valueName: str):
        import winreg
        try:
            with self.__OpenKey(winreg.HKEY_LOCAL_MACHINE, subkey) as hKey:
                return winreg.QueryValueEx(hKey, valueName)[0]
        except:
            sys.stderr.write("\rCouldn't find the value with name: {}\n".format(valueName))
            return None
    
class LinuxRegistry(Registry):
    def QueryValue(self, subkey: str, valueName: str):
        try:
            return os.environ.get(subkey)
        except:
            sys.stderr.write("\rCouldn't find the value with name: {}\n".format(subkey))
            return None
    
def CreateRegistry() -> Registry:
    registries = {
        'nt': WindowsRegistry,
        'posix': LinuxRegistry
    }
    return registries[os.name]()
