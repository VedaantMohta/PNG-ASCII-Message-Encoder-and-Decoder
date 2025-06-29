# Image Encoding and Decoding Project

## 📌 Overview
A high-performance C-based CLI tool that securely embeds and extracts ASCII messages from PNG images using Least Significant Bit (LSB) steganography. Designed for precision, resilience, and performance, this project showcases strong command over systems programming, memory management, and image-level data manipulation. Built from scratch in C with no external dependencies beyond stb_image, reflecting hands-on understanding of raw data processing.

## ⚙️ Features
- 📦 Invisible Data Embedding: Alters only the least significant bits of each pixel to hide messages without affecting image quality.
- 🧪 Built-in Data Integrity: Appends an 8-bit XOR checksum to detect and flag any message corruption during decoding.
- 🎯 Marker-Based Extraction: Utilizes a fixed end-of-message marker (00000111) — no need to store message length.
- 💡 Capacity Awareness: Dynamically calculates and displays max encodable characters based on image size.
- 🧼 Memory-Safe Design: Implements centralized cleanup paths, pointer nulling, and error fallback logic to prevent memory leaks.
- 🧰 CLI UX Optimized: Interactive prompts with real-time feedback, input validation, and graceful termination ('q' to quit).

## 🛠️ Design Decisions

### Choice of Encoding Method
The encoding method used in this project is a substitution technique where the least significant bit (LSB) of each pixel in the image is replaced with a bit of the message. This method was chosen because:
- It introduces minimal changes to the image, making the hidden message less noticeable. Since only one LSB is modified, the values of RGBA are only altered by 1 out of 255.
- It is efficient in terms of processing time and computational resources.

### Data Integrity: XOR Checksum
Before embedding, a simple XOR checksum is computed over the entire binary message and appended. Upon decoding, the checksum is recalculated and compared to detect corruption — ensuring message reliability without external libraries.

### Message Demarcation: End Marker
A fixed 8-bit binary marker (00000111, ASCII Bell) is embedded after the checksum. This enables autonomous decoding, removes dependency on message length, and provides a robust termination point, ensuring no resources are wasted past the end of the message.

### Image Processing: stb_image
The project uses the [stb_image](https://github.com/nothings/stb) library for loading and writing PNG images. The stb_image library was chosen because:
- Header-only simplicity — no linking required.
- Raw pixel access — essential for precise bit manipulation.
- Cross-platform compatibility — works on Windows, macOS, Linux.

### Language: C
C was selected for performance and control:
- Manual memory management: demonstrates mastery of malloc, free, and pointer safety.
- Efficient error handling: structured goto usage enables centralized, safe cleanup.
- Bitwise control: reinforces deep understanding of binary encoding and data-level manipulation.

## Platform
The program was tested on the following platform:
- Windows 11

## 🚀 Build and Run
### 🔧 Prerequisites
- GCC or compatible C compiler
- `stb_image.h` and `stb_image_write.h` in a `stb_image_library/` folder at the root
- Additionally, the image you wish to encode or decode must be in the same folder as the code files.

### Makefile
- 📦 Compilation: Utilize the command "Make" to compile the program. It will use the gcc compiler along with predetermined tags.
- ▶️ Running: Utilize the command "Make run" to run the program. It will run the file Steganography_CLI_Tool.exe, which is produced from compilation.
- 🧹 Cleaning: Utilize the command "Make clean" to clean files. It will remove Steganography_CLI_Tool.exe

### Manually
- 📦 Compilation: gcc -Wall -g Steganography_CLI_Tool.c -o Steganography_CLI_Tool -I./stb_image_library
- ▶️ Running: .\Steganography_CLI_Tool.exe for Windows or ./Steganography_CLI_Tool for Linux/macOS
- 🧹 Cleaning: del Steganography_CLI_Tool.exe for Windows or rm Steganography_CLI_Tool for Linux/macOS
