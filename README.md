# IoT Drive

A virtual drive implementation that uses a regular Linux directory as storage while distributing the actual file data across network-connected IoT devices in a home network. Designed to maximize storage utilization and improve security through decentralized file storage.

## Features
- Store files on available IoT devices in the local network.
- Transparent Linux directory interface for easy user interaction.
- Basic security measures to prevent unauthorized access.
- Multi-threaded file management and network communication.

## Technologies Used
- **C++** – System programming and file management.
- **Linux Sockets** – TCP/UDP communication between devices.
- **POSIX Threads** – Multi-threading for parallel file handling.
- **File System API** – Reading/writing files and directories.

## Architecture
1. **Client-side** – Acts as a virtual file system, intercepting read/write operations.
2. **Server-side (IoT devices)** – Stores received file chunks and responds to retrieval requests.
3. **Coordinator** – Maintains mapping between files and storage locations.
