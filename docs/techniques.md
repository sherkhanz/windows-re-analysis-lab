# Techniques

## Summary
- **Anti-Debugging**: Actively checks for debuggers using both standard Windows APIs and direct Process Environment Block (PEB) inspection.
- **Anti-VM**: Detects sandbox environments by comparing network adapter MAC address prefixes against known VMware and VirtualBox identifiers.
- **Anti-Disassembly**: Employs opaque predicates and junk byte insertion to disrupt static analysis and linear disassemblers.
- **Persistence Mechanisms**: Establishes reboot survival by masquerading as a Windows update executable and modifying the Registry Run key.
- **Benign Ransomware Payload**: Simulates cryptographic ransomware behavior by encrypting strings in memory using the Windows CryptoAPI and RC4 algorithm.
- **Deception Strategy**: Embeds a list of fake Indicators of Compromise (IOCs) to mislead automated string extraction tools.

---

## Anti-Debugging

**Description**: The application identifies if it is being executed within a debugger to prevent dynamic analysis.

**How it is implemented in `main.c`**  
- `main()` checks two conditions:
  - `IsDebuggerPresent()` (Windows API)
  - `CheckPEB_BeingDebugged()` (direct PEB flag inspection via inline assembly)
- If either condition is true, the program displays a mocking Top-Most MessageBox ("Debugger Detected! Nice try...") and immediately terminates, blocking further dynamic analysis.
