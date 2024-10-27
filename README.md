# Darken Anticheat
Open source solution for protecting your software against tampering and reverse engineering.

# Current Features
- Detects debugged attached via the PEB's `BeingDebugged` byte.
- Detects dlls which not digitally signed or have a PDB in microsoft's symbol server.