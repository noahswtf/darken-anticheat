# Darken Anticheat
Open source solution for protecting your software against tampering and reverse engineering.

# Current Features
- Detects dlls which not digitally signed or have a PDB in microsoft's symbol server.
- Strips permissions of handles that are attempted to be opened to any of our protected processes.
- Detects system threads which have a start address outside of any legitimate kernel module.
- Detects system threads which are attached to any of our protected processes.
- Detects process threads which have a start address outside of any legitimate module in the respective protected process.
- Detects debugged attached via the PEB's `BeingDebugged` byte.
- Force-triggering PatchGuard (KPP) checks to run (causes bugcheck to occur if there are any violations of PatchGuard's checks).
- Causing bugcheck if PatchGuard (KPP) is disabled / has an invalid context.
- Checking usage of reserved MSRs (model specific registers).
- Resolving all driver imports manually.

# Credits
- [papstuc](https://github.com/papstuc) for the [nocrt portable executable library used](https://github.com/papstuc/nocrt_portable_executable).

# Licensing
This project has been placed under the [GNU General Public License](LICENSE), if there are any specific enquiries regarding usage, please contact the founder of the project (noahswtf).