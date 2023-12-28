# Darken Anticheat
Free and open source solution for protecting your software/games against attackers.

# Current Features
- Suspicious Process Module Scanning
- Handle Permission Stripping
- Suspicious Process Thread Scanning
- Suspicious Kernel Thread Scanning
- Detecting Execution Under A Virtual Machine

# How To Use

To execute an unsigned driver, you'll first need to enable [test signing](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/test-signing) on your Windows machine. Follow the steps in the hyperlink to enable said setting.
You also need [Microsoft Visual Studio](https://visualstudio.microsoft.com/) to open the project and [Windows Driver Standard Development Kit](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk) and respective Visual Studio C/C++ compilers to be able to compile the driver.

Once you have compiled the project, you need to install the driver as a system service. Instructions on how to do so from the command line are at [System Control](https://learn.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2012-r2-and-2012/cc754599(v=ws.11)).

Once you have started the service, just run the user-mode application, and it will initialise the protected process and start to communicate with the driver to run checks.

# Plans for the future

I will continue to update the driver, and add many other features. If you have any suggestions then feel free to tell me. I'd like this to become a project useful for people wanting to learn about how anticheats work and a free solution for companies to protect their applications from attackers.

# Licensing
This project falls under the [GNU General Public License](LICENSE), if there are any specific enquiries regarding usage, please contact the founder of the project.
