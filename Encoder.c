#define STB_IMAGE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include "stb_image_library/stb_image.h"
#include "stb_image_library/stb_image_write.h"

void write_binary_data_to_file(unsigned char *image, int width, int height, int channels, const char *filename) {
    FILE *file = fopen(filename, "w"); //Open a file in write mode
    if (file == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                unsigned char pixel_value = image[(y * width + x) * channels + c];

                // Write binary of each pixel channel (Ex: RGB) to the file
                for (int i = 7; i >= 0; i--) {
                    fprintf(file, "%d", (pixel_value >> i) & 1);
                }
                fprintf(file, " "); // Separate channels by space
            }
            fprintf(file, "\n"); // Newline for each pixel
        }
    }

    fclose(file);  // Close the file
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
            messageBinary[i * 9 + 7 - j] = ((message[i] >> j) & 1) ? '1' : '0';
        }
        messageBinary[i * 9 + 8] = ' ';
    }

    messageBinary[binaryLength - 1] = '\0';  // Null-terminate the string
    return messageBinary;
}

void encode_message(const char *filename, const char *message) {

}


int main() {
    // Load the PNG image using stb_image
    int width, height, channels;
    const char *input_filename = "images/IMAGEASCII.png"; 
    const char *output_filename = "binary_data.txt"; // Output text file containing binary data of image

    unsigned char *image = stbi_load(input_filename, &width, &height, &channels, 0);
    if (image == NULL) {
        printf("Failed to load image\n");
        return 1;
    }

    printf("Image loaded successfully! %d x %d with %d channels\n", width, height, channels);
    
    // Write the binary values of each pixel to a text file
    write_binary_data_to_file(image, width, height, channels, output_filename);

    printf("Binary data written to %s\n", output_filename);

    // Free the image after use
    stbi_image_free(image);
    printf("Conversion");
    printf("%s", messageToBinary("Hello, my name is Vedaant. What is your name?"));

    return 0;
}
