# Techniques

## Summary
- **Anti-Debugging**: Actively checks for debuggers using both standard Windows APIs and direct Process Environment Block (PEB) inspection.
- **Anti-VM**: Detects sandbox environments by comparing network adapter MAC address prefixes against known VMware and VirtualBox identifiers.
- **Anti-Disassembly**: Employs opaque predicates and junk byte insertion to disrupt static analysis and linear disassemblers.
- **Persistence Mechanisms**: Establishes reboot survival by masquerading as a Windows update executable and modifying the Registry Run key.
- **Benign Ransomware Payload**: Simulates cryptographic ransomware behavior by encrypting strings in memory using the Windows CryptoAPI and RC4 algorithm.
- **Deception Strategy**: Embeds a list of fake Indicators of Compromise (IOCs) to mislead automated string extraction tools.

## Anti-Debugging

