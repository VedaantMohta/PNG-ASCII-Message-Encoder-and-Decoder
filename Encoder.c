#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stb_image_library/stb_image.h"
#include "stb_image_library/stb_image_write.h"

// Convert an image to binary representation (stored in memory)
char *image_to_binary(unsigned char *image, int width, int height, int channels, int *binary_size) {
    int buffer_size = width * height * channels * 8;  // 8 bits per pixel + spaces
    char *binary_data = (char *)malloc(buffer_size);
    if (!binary_data) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                unsigned char pixel_value = image[(y * width + x) * channels + c];

                // Store each pixel channel as binary
                for (int i = 7; i >= 0; i--) {
                    binary_data[index++] = ((pixel_value >> i) & 1) ? '1' : '0';
                }
            }
        }
    }
    *binary_size = index;
    return binary_data;
}

char *messageToBinary(const char *message) {
    int length = strlen(message);
    int binaryLength = length * 8 + 1;

    char *messageBinary = (char *)malloc(binaryLength);
    if (!messageBinary) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    for (int i = 0; i < length; i++) {
        for (int j = 7; j >=0; j--) {
            messageBinary[i * 8 + 7 - j] = ((message[i] >> j) & 1) ? '1' : '0';
        }
    }

    messageBinary[binaryLength - 1] = '\0';  // Null-terminate the string
    return messageBinary;
}

char *encode_binary(char *binary_data, const char *message) {
    int i = 0; // Iterator for message

    while (message[i] != '\0') {
        binary_data[(i + 1) * 8 - 1] = message[i];
        i++;
    }
    int j = 0;
    char end[] = "00000111"; 
    while (end[j] != '\0') {
        binary_data[(i + 1) * 8 - 1] = end[j];
        i++;
        j++;
    }

    return binary_data;
}

// Convert binary representation back to an image
unsigned char *binary_to_image(char *binary_data, int width, int height, int channels) {
    unsigned char *image = (unsigned char *)malloc(width * height * channels);
    if (!image) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int binary_index = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                unsigned char pixel_value = 0;
                for (int i = 0; i < 8; i++) {
                    pixel_value = (pixel_value << 1) | (binary_data[binary_index++] - '0');
                }
                image[(y * width + x) * channels + c] = pixel_value;
            }
        }
    }
    return image;
}

char *decode_image(char *image, int width, int height, int channels) {
    int length = strlen(image);
    int result_length = (length / 8); // Every 8th bit, rounding up
    char *result = (char *)malloc(result_length + 1);  // +1 for null-terminator

    if (!result) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int result_index = 0;
    char byte[9];
    for (int i = 7; i < length; i += 8) {
        
        
        result[result_index++] = image[i];


        if ((result_index) % 8 == 0) {
            strncpy(byte, result + result_index - 8, 8);
            byte[8] = '\0';
            if (strcmp(byte, "00000111") == 0) {
                result[result_index - 8] = '\0';  // Null-terminate the result
                return result;
            }
        }
    }

    result[result_index] = '\0';  // Null-terminate the result
    return result;
}

char *binaryToAscii(const char *binary_data) {
    int length = strlen(binary_data);
    if (length % 8 != 0) {
        printf("Error: Binary data length should be a multiple of 8.\n");
        return NULL;
    }

    int asciiLength = length / 8;
    char *ascii_data = (char *)malloc(asciiLength + 1);
    if (!ascii_data) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    for (int i = 0; i < asciiLength; i++) {
        char ascii_char = 0;
        for (int j = 0; j < 8; j++) {
            ascii_char = ascii_char << 1;
            if (binary_data[i * 8 + j] == '1') {
                ascii_char = ascii_char | 1;
            }
        }
        ascii_data[i] = ascii_char;
    }

    ascii_data[asciiLength] = '\0'; // Null-terminate the string
    return ascii_data;
}

int main() {
    int width, height, channels;
    const char *input_filename = "images/IMAGEASCII.png"; 
    const char *output_filename = "output_image.png"; // Output reconstructed image

    // Load the PNG image using stb_image
    unsigned char *image = stbi_load(input_filename, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Failed to load image\n");
        return 1;
    }

    printf("Image loaded successfully! %d x %d with %d channels\n", width, height, channels);

    // Convert image to binary representation
    int binary_size;
    char *binary_data = image_to_binary(image, width, height, channels, &binary_size);
    if (!binary_data) {
        stbi_image_free(image);
        return 1;
    }
    
    // Free original image memory
    stbi_image_free(image);
    char *binary_message = messageToBinary("Congratulations again on completing your 2nd round technical interview! The last step of the interview process for the Flight Software team is this take-home project. You will have 7 days to complete this project, and it will be due at 11:59 PM EDT, on Thursday, February 20th. Please make your submissions via email to sl94656@uga.edu and include all your project files in a ZIP archive. This project will involve encoding an ASCII message in a PNG image without (noticeably) visually altering the image. You may choose the method by which you encrypt your data, but make a conscious effort to develop an efficient encoding method (how much text data can you pack in a single image?) and you may not utilize the metadata of the PNG to store information. The message may be one of your choosing, and the PNG image with which you will be working is attached alongside this document to the email you received. The project will be written in the C language, and your project should consist of two separate programs, one to encode the data in the image, and another to decode the data from the image. You will need to include a makefile such that a user can run 'make clean' and 'make in the target directory and successfully compile your code. You will also include a README.md file in which you will describe how your projects work, as well as why you made certain design decisions (such as choice of encoding method, choice of data redundancy, etc.). You may use a PNG library of your choosing but be sure to justify your choice in your README. It is alright if your program is not cross-platform, but please include any necessary libraries for compilation and include which platform your program was tested on (Windows, Linux, MacOS). Your program must include some way for the decoder to detect any errors during transmission in the encoded data. (Bonus points if you implement an error correction method, but this is not required).");
    
    encode_binary(binary_data, binary_message);
    
    // Convert binary data back to an image
    unsigned char *reconstructed_image = binary_to_image(binary_data, width, height, channels);
    free(binary_data);  // Free binary memory

    if (!reconstructed_image) {
        return 1;
    }

    // Save the reconstructed image
    if (!stbi_write_png(output_filename, width, height, channels, reconstructed_image, width * channels)) {
        printf("Failed to write image\n");
        free(reconstructed_image);
        return 1;
    }

    printf("Reconstructed image saved to %s\n", output_filename);

    // Free memory
    free(reconstructed_image);




    int width_1, height_1, channels_1;
    const char *input_filename_1 = "output_image.png"; 

    // Load the PNG image using stb_image
    unsigned char *image_1 = stbi_load(input_filename_1, &width_1, &height_1, &channels_1, 0);
    if (image_1 == NULL) {
        printf("Failed to load image\n");
        return 1;
    }

    printf("Image loaded successfully! %d x %d with %d channels\n", width_1, height_1, channels_1);

    // Convert image to binary representation
    int binary_size_1;
    char *binary_data_1 = image_to_binary(image_1, width_1, height_1, channels_1, &binary_size_1);
    if (!binary_data_1) {
        stbi_image_free(image_1);
        return 1;
    }
    
    // Free original image memory
    stbi_image_free(image_1);
    
    // Decode the image to get the binary message
    char *message_binary_1 = decode_image(binary_data_1, width_1, height_1, channels_1);
    if (!message_binary_1) {
        free(binary_data_1);
        return 1;
    }
    
    // Convert the binary message to ASCII
    char *ascii_message_1 = binaryToAscii(message_binary_1);
    if (!ascii_message_1) {
        free(binary_data_1);
        free(message_binary_1);
        return 1;
    }



    printf("Decoded ASCII message: %s\n", ascii_message_1);

    // Free allocated memory
    free(binary_data_1);
    free(message_binary_1);
    free(ascii_message_1);




    return 0;
}
