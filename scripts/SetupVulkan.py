import os
import sys
import subprocess
import Utils

class VulkanSetup:

    vulkanVersion = "1.3.268.0"
    vulkanFileName = {
        "linux": f"vulkansdk-linux-x86_64-{vulkanVersion}.tar.xz"
    }
    vulkanUrl = {
        "linux": f"https://sdk.lunarg.com/sdk/download/{vulkanVersion}/linux/{vulkanFileName['linux']}"
    }
    vulkanDownloadDirectory = os.path.expanduser("~/vulkan")

    vulkanInstallDirectory = {
        "linux": os.path.expanduser(f"{vulkanDownloadDirectory}/{vulkanVersion}/")
    }

    @classmethod
    def Setup(self):
        if not os.path.exists(self.vulkanInstallDirectory[sys.platform]):
            self.InstallVulkan()
        else:
            print("Vulkan  SDK aready installed!")

    @classmethod
    def InstallVulkan(self):
        if not os.path.exists(self.vulkanDownloadDirectory):
            os.makedirs(self.vulkanDownloadDirectory)

        vulkanPath = f"{self.vulkanDownloadDirectory}/{self.vulkanFileName[sys.platform]}"

        Utils.DownloadFile(self.vulkanUrl[sys.platform], vulkanPath)
        print("Running Vulkan SDK installer...")

        if sys.platform == "linux":
            subprocess.call(["xdg-open", os.path.abspath(vulkanPath)])

        print("Re-run script after install")
        quit()
