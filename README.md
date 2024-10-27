# Darken Anticheat
Open source solution for protecting your software against tampering and reverse engineering.

# Current Features
- Detects debugged attached via the PEB's `BeingDebugged` byte.
- Detects dlls which not digitally signed or have a PDB in microsoft's symbol server.
- Strips permissions of handles that are attempted to be opened to either the anticheat's usermode process or the protected process.

# Licensing
This project has been placed under the [GNU General Public License](LICENSE), if there are any specific enquiries regarding usage, please contact the founder of the project (noahswtf).