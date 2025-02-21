#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stb_image_library/stb_image.h"
#include "stb_image_library/stb_image_write.h"

// Convert an image to binary representation (stored in memory)
char *image_to_binary(unsigned char *image, int width, int height, int channels, int *binary_size) {
    int buffer_size = width * height * channels * 8 + width * height;  // 8 bits per pixel + spaces
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
    int binaryLength = length * 9 + 1;

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
    char *binary_message = messageToBinary("Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? Hello, my name is Vedaant. What is your name? ");
    printf("Before Encoding:\n");
    printf("Substring: %.*s\n", 64, binary_data); 
    
    encode_binary(binary_data, binary_message);
    printf("After Encoding:\n");
    printf("Substring: %.*s\n", 64, binary_data);    
    
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

    return 0;
}
