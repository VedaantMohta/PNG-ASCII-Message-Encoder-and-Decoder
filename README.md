# Image Encoding and Decoding Project

## Overview
This project implements an image steganography tool that encodes a hidden message into an image and decodes it back from the image. The program allows for seamless conversion of text messages to binary, encoding into image binary data, and reconstructing the image with the hidden message. Additionally, it calculates checksums to ensure data integrity during the encoding and decoding processes.

## Features
- Load PNG images for encoding and decoding.
- Convert images to binary data.
- Encode text messages into binary data of images.
- Decode hidden messages from binary data of images.
- Calculate checksums to verify data integrity of the images before and after encoding.

## Design Decisions

### Choice of Encoding Method
The encoding method used in this project is a simple substitution technique where the least significant bit (LSB) of each pixel in the image is replaced with a bit of the message. This method was chosen because:
1. It is straightforward to implement.
2. It introduces minimal changes to the image, making the hidden message less noticeable. Since only one LSB is modified, the values of RGBA are only altered by 1 out of 255.
3. It is efficient in terms of processing time and computational resources.

### Choice of Data Redundancy
Data redundancy is introduced by adding a unique sequence ("00000111") at the end of the encoded message. This allows the decoding function to identify the end of the hidden message, ensuring that only the relevant data is extracted and reduces runtime significantly. This choice balances between simplicity and reliability.

### PNG Library
The project uses the [stb_image](https://github.com/nothings/stb) library for loading and writing PNG images. The stb_image library was chosen because:
1. It is lightweight and easy to integrate.
2. It supports multiple image formats, including PNG.
3. It does not require any external dependencies, making it suitable for cross-platform development.

### Platform
The program was tested on the following platform:
- Windows 11

## Prerequisites
To compile and run the program, you will need the following libraries:
- stb_image.h
- stb_image_write.h

## Makefile
- Compilation: Utilize the command "Make" to compile the program. It will use the gcc compiler along with predetermined tags.
- Running: Utilize the command "Make run" to run the program. It will run the file Encoder.exe, which is produced from compilation.
- Cleaning: Utilize the command "Make clean" to clean files. It will remove Encoder.exe and output_image.png