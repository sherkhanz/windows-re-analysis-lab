# Windows RE Analysis Lab

The **Windows RE Analysis Lab** is an educational Windows reverse engineering project created to simulate and analyze behavior patterns commonly observed in malware samples.

## Core Capabilities

- **Anti-Debugging:** Actively detects debugger presence using both standard Windows APIs (`IsDebuggerPresent`) and direct, hook-evading Process Environment Block (PEB) inspection via inline assembly (`$FS:[0x30]`).
- **Anti-VM:** Validates the execution environment by comparing network adapter MAC address prefixes against known virtualization vendors (VMware, VirtualBox), triggering a disruptive GDI screen-melting effect if a sandbox is detected.
- **Anti-Disassembly:** Employs opaque predicates and rogue junk byte insertion (`0xE8`) to disrupt control-flow graph (CFG) reconstruction and break linear disassembly in tools like IDA Pro and Ghidra.
- **Persistence Mechanisms:** Demonstrates autostart execution by masquerading as a Windows update executable and modifying the `HKCU\Software\Microsoft\Windows\CurrentVersion\Run` registry key.
- **Ransomware Simulation:** Safely emulates ransomware behavior by utilizing the native Windows Cryptography API (CryptoAPI) to encrypt a designated in-memory string using RC4, followed by a non-destructive visual payload (cascading ransom-style popups).
- **Embedded Fake IOCs:** Integrates a curated array of deceptive static artifacts (fake URLs, base64 strings, CVE references, and PowerShell commands) to demonstrate the pitfalls of automated string extraction during static triage.

## Preview

### Anti-Debugging Triggered
![Anti-Debugging Triggered](screenshots/01_anti_debugging.png)

*Execution under a debugger triggers the anti-debugging branch and terminates the sample early.*

### Registry Run Key Persistence
![Registry Run Key Persistence](screenshots/04_persistence_registry.png)

*The sample creates a `Run` key entry to launch automatically at user logon.*

### Ransomware Simulation Triggered
![Ransomware Simulation Triggered](screenshots/03_ransomware_simulation.png)

*The sample simulates ransomware behavior by displaying repeated ransom-themed popups after encrypting an in-memory test string using the Windows CryptoAPI.*

## Full Technical Documentation

Detailed technique-by-technique analysis is available here:

**[docs/techniques.md](docs/techniques.md)**

## Repository Structure

```text
windows-re-analysis-lab/
├── README.md               # Repository overview and guidelines
├── LICENSE                 # MIT License
├── src/
│   └── main.c              # Core simulation source code
├── docs/
│   └── techniques.md       # Detailed technical analysis and methodology
└── screenshots/            # Evidence of successful technique execution
