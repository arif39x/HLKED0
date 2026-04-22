# HLKED0: T

## LEGAL & ETHICAL DISCLAIMER

**This repository is for strictly EDUCATIONAL and RESEARCH purposes only.**

The contents of this repository include raw source code for historical and contemporary malware, exploits, and command-and-control (C2) frameworks. This collection is intended to provide security researchers, students, and analysts with a platform to study the mechanics, logic, and development patterns of malicious software to better defend systems globally.

**The repository owner and contributors assume NO responsibility for any unethical or illegal work performed using the code in this repository.** Use of this material for attacking targets without prior mutual consent is illegal. It is the end user's responsibility to obey all applicable local, state, and federal laws.

---

## Repository Overview

Project HLKED0 is a massive, multi-language dump of malware source code. It covers the entire lifecycle of a cyberattack—from initial exploit and payload delivery to persistent rootkits and full-scale botnet management.

The repository is structured by language and function, featuring a significant amount of historical leak data and modern implementations.

### What’s Inside?

#### Binary Infectors & Worms

Tools designed to spread autonomously or modify existing binaries.

- **d0zer:** A Go-based ELF infector using the Silvio Cesar padding algorithm.
- **I-Worm Collection:** Historical C-based internet worms like Poffer and Wozer.
- **Unix Parasites:** Assembly-level implementations of ELF infection techniques.

#### Botnet Frameworks & DDoS Tools

Complete source code for managing zombie networks.

- **Mirai & Variants:** Source code for Moobot and other Mirai-based IoT bots.
- **Classic Bots:** Source code for rBot, Kaiten, Qbot (Bashlite), and rBot.
- **DDoS Engines:** Specialized flooders for TCP, UDP, and HTTP-based attacks.

#### Rootkits & Persistence

Code designed to hide presence on a system or maintain access.

- **Kernel Rootkits:** Implementations of Adore, Knark, and Alpha for Linux.
- **Windows Rootkits:** User-mode and kernel-mode hooks (e.g., Agony).

#### Information Stealers & Spyware

Tools built to harvest data from victims.

- **Credential Grabbers:** iStealer, CertGrab, and various FTP/HTTP sniffers.
- **Keyloggers:** Low-level hook-based loggers for Windows.
- **HVNC:** Hidden Virtual Network Computing for ghost access to user desktops.

#### Command & Control (C2) Panels

The brains of the operation—web-based dashboards for attackers.

- **PHP Backends:** Massive collection of panels for Zeus, Mirai, and various PHP-based gates.
- **Exploit Kits:** Web-based scripts targeting Adobe, Java, and specific router vulnerabilities (Gpon, Huawei, Netis).

#### Exploit Scripts & Payloads

- **Python/Perl Scripts:** Automation for scanning ports, deploying payloads, and installing Qbot.
- **Assembly Shellcode:** Optimized payloads for Win32 and Linux x86/x64 architectures.

---

## Technical Landscape

This repository serves as a cross-platform study guide, including:

- **Languages:** Assembly (x86/x64), C, C++, Go, PHP, Python, Perl, Ruby, and Java/Smali.
- **Platforms:** Windows (NT/XP/7/10), Linux (Server/IoT), and Mobile (Android).
- **Techniques:** Hooking, process injection, ELF/PE infection, obfuscation, and C2 communication protocols.

---

## Research Goals

Researchers are encouraged to use this repository to:

- **Develop Signatures:** Create YARA, Snort, or ClamAV rules for detecting these threats.
- **Analyze Logic:** Understand how malware authors bypass modern defenses.
- **Reverse Engineering:** Practice de-obfuscation and behavioral analysis in a controlled environment.

---
