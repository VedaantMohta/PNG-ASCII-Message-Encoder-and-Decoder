#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stb_image_library/stb_image.h"
#include "stb_image_library/stb_image_write.h"

// Function prototypes
/**
 * Converts binary data into an ASCII string.
 *
 * @param binary_data The binary data to be converted into ASCII.
 * @return The corresponding ASCII string.
 */
char *binaryToAscii(const char *binary_data);

/**
 * Converts a given message into its binary representation.
 *
 * @param message The ASCII message to be converted into binary.
 * @return The binary representation of the message as a string.
 */
char *messageToBinary(const char *message);

/**
 * Calculates a simple XOR checksum over a given data.
 *
 * @param data The data to calculate the checksum over.
 * @param length The length of the data.
 * @return The XOR checksum of the data.
 */
unsigned char calculate_checksum(const char* data, size_t length) {
    unsigned char checksum = 0;
    // Iterate over the data and apply XOR operation to each byte
    for (size_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * Converts an image into its binary representation.
 *
 * @param image The image data.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @param binary_size A pointer to store the size of the resulting binary data.
 * @return The binary representation of the image.
 */
char *image_to_binary(unsigned char *image, int width, int height, int channels, int *binary_size) {
    int buffer_size = width * height * channels * 8;  // Each pixel's value represented in 8 bits
    char *binary_data = (char *)malloc(buffer_size);
    
    if (!binary_data) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int index = 0;
    // Convert each pixel value to its binary equivalent
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                unsigned char pixel_value = image[(y * width + x) * channels + c];
                for (int i = 7; i >= 0; i--) {
                    binary_data[index++] = ((pixel_value >> i) & 1) ? '1' : '0';
                }
            }
        }
    }
    *binary_size = index;  // Store the size of the binary data
    return binary_data;
}

/**
 * Converts a message (string) into its binary representation.
 *
 * @param message The message to be converted into binary.
 * @return A string containing the binary representation of the message.
 */
char *messageToBinary(const char *message) {
    int length = strlen(message);
    int binaryLength = length * 8 + 1;  // Each character is 8 bits

    char *messageBinary = (char *)malloc(binaryLength);
    if (!messageBinary) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    // Convert each character of the message into its binary form
    for (int i = 0; i < length; i++) {
        for (int j = 7; j >= 0; j--) {
            messageBinary[i * 8 + 7 - j] = ((message[i] >> j) & 1) ? '1' : '0';
        }
    }

    messageBinary[binaryLength - 1] = '\0';  // Null-terminate the string
    return messageBinary;
}

/**
 * Encodes a binary string with the given message and appends the checksum.
 *
 * @param binary_data The binary data to which the message is to be encoded.
 * @param message The message to be encoded in binary.
 * @return The binary string with the encoded message and checksum appended.
 */
char *encode_binary(char *binary_data, const char *message) {
    // Convert message to binary
    char *binary_message = messageToBinary(message);
    unsigned char checksum = calculate_checksum(binary_message, strlen(binary_message));
    free(binary_message);  // Free the binary message as it's no longer needed
    
    int i = 0;
    // Encode the message in binary and append to binary_data
    while (message[i] != '\0') {
        binary_data[(i + 1) * 8 - 1] = message[i];
        i++;
    }
    
    // Convert checksum to binary and append to binary_data
    for (int j = 0; j < 8; j++) {
        binary_data[(i + 1) * 8 - 1] = ((checksum >> (7 - j)) & 1) ? '1' : '0';
        i++;
    }
    
    // Append an end marker (8 bits)
    char end[] = "00000111";
    int j = 0;
    while (end[j] != '\0') {
        binary_data[(i + 1) * 8 - 1] = end[j];
        i++;
        j++;
    }

    return binary_data;
}

/**
 * Converts binary data back into an image representation.
 *
 * @param binary_data The binary data to convert into an image.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @return The image data created from the binary data.
 */
unsigned char *binary_to_image(char *binary_data, int width, int height, int channels) {
    unsigned char *image = (unsigned char *)malloc(width * height * channels);
    
    if (!image) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int binary_index = 0;
    // Convert binary data back to image format
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

/**
 * Decodes the binary data embedded in an image and extracts the message.
 *
 * @param image The binary data embedded in the image.
 * @param width The width of the image.
 * @param height The height of the image.
 * @param channels The number of channels in the image.
 * @return The decoded message in binary format.
 */
char *decode_image(char *image, int width, int height, int channels) {
    int length = strlen(image);
    int result_length = (length / 8);
    char *result = (char *)malloc(result_length + 1);
    
    if (!result) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int result_index = 0;
    char byte[9];
    unsigned char extracted_checksum = 0;
    char *message_content = NULL;
    
    for (int i = 7; i < length; i += 8) {
        result[result_index++] = image[i];

        if ((result_index) % 8 == 0) {
            strncpy(byte, result + result_index - 8, 8);
            byte[8] = '\0';
            
            // Check for end marker in the binary data
            if (strcmp(byte, "00000111") == 0) {
                // Extract message (excluding checksum and end marker)
                int message_length = result_index - 16; // 8 bits for checksum, 8 bits for end marker
                message_content = (char *)malloc(message_length + 1);
                memcpy(message_content, result, message_length);
                message_content[message_length] = '\0';
                
                // Extract checksum (8 bits) from the decoded data
                for (int j = 0; j < 8; j++) {
                    extracted_checksum = (extracted_checksum << 1) | (result[message_length + j] - '0');
                }
                
                // Calculate checksum of the message content
                unsigned char calculated_checksum = calculate_checksum(message_content, strlen(message_content));
                
                // Verify the checksum
                if (calculated_checksum != extracted_checksum) {
                    printf("Warning: Checksum verification failed! Message may be corrupted.\n");
                    printf("Expected checksum: %02X, Got: %02X\n", extracted_checksum, calculated_checksum);
                } else {
                    printf("Checksum verification successful!\n");
                }
                
                result[message_length] = '\0';
                free(message_content);
                return result;
            }
        }
    }

    result[result_index] = '\0';
    return result;
}

/**
 * Converts binary data into its corresponding ASCII characters.
 *
 * @param binary_data The binary data to be converted.
 * @return The corresponding ASCII representation of the binary data.
 */
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

    // Convert each 8-bit segment into a corresponding ASCII character
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

    ascii_data[asciiLength] = '\0';  // Null-terminate the string
    return ascii_data;
}

int main() {
    // Declare variables to hold image properties: width, height, and number of channels
    int width, height, channels;

    // Define file names for input and output images
    const char *input_filename = "images/IMAGEASCII.png";  // Input image
    const char *output_filename = "output_image.png";      // Output image after encoding the message

    // Load the image using the stb_image library
    unsigned char *image = stbi_load(input_filename, &width, &height, &channels, 0);
    if (image == NULL) {  // Check if image loading was successful
        printf("Failed to load image\n");  // Display an error message if the image could not be loaded
        return 1;  // Return non-zero to indicate failure
    }

    // If the image loaded successfully, display the image dimensions and number of channels
    printf("Image loaded successfully! %d x %d with %d channels\n", width, height, channels);

    // Initialize a variable to hold the size of the binary data
    int binary_size;
    
    // Convert the image to binary data (encoding the pixel values)
    char *binary_data = image_to_binary(image, width, height, channels, &binary_size);
    if (!binary_data) {  // If conversion fails, free the loaded image and return failure
        stbi_image_free(image);  // Free the memory used by the loaded image
        return 1;
    }
    
    // Free the image memory as it is no longer needed
    stbi_image_free(image);

    // Convert the message into a binary format (this will be encoded in the image)
    char *binary_message = messageToBinary("Congratulations again on completing your 2nd round technical interview! The last step of the interview process for the Flight Software team is this take-home project. You will have 7 days to complete this project, and it will be due at 11:59 PM EDT, on Thursday, February 20th. Please make your submissions via email to sl94656@uga.edu and include all your project files in a ZIP archive. This project will involve encoding an ASCII message in a PNG image without (noticeably) visually altering the image. You may choose the method by which you encrypt your data, but make a conscious effort to develop an efficient encoding method (how much text data can you pack in a single image?) and you may not utilize the metadata of the PNG to store information. The message may be one of your choosing, and the PNG image with which you will be working is attached alongside this document to the email you received. The project will be written in the C language, and your project should consist of two separate programs, one to encode the data in the image, and another to decode the data from the image. You will need to include a makefile such that a user can run 'make clean' and 'make' in the target directory and successfully compile your code. You will also include a README.md file in which you will describe how your projects work, as well as why you made certain design decisions (such as choice of encoding method, choice of data redundancy, etc.). You may use a PNG library of your choosing but be sure to justify your choice in your README. It is alright if your program is not cross-platform, but please include any necessary libraries for compilation and include which platform your program was tested on (Windows, Linux, MacOS). Your program must include some way for the decoder to detect any errors during transmission in the encoded data. (Bonus points if you implement an error correction method, but this is not required).");
    // Pass the binary data and the message binary data to the encoder function
    encode_binary(binary_data, binary_message);

    // Free the binary message memory after it's no longer needed
    free(binary_message);
    
    // Reconstruct the image with the encoded message from the binary data
    unsigned char *reconstructed_image = binary_to_image(binary_data, width, height, channels);
    // Free the binary data since it's now embedded in the image
    free(binary_data);

    if (!reconstructed_image) {  // If image reconstruction fails, return failure
        return 1;
    }

    // Attempt to save the reconstructed image as a PNG file
    if (!stbi_write_png(output_filename, width, height, channels, reconstructed_image, width * channels)) {
        // If saving the image fails, print an error message and free the reconstructed image
        printf("Failed to write image\n");
        free(reconstructed_image);
        return 1;
    }

    // If the image was saved successfully, print a success message
    printf("Reconstructed image saved to %s\n", output_filename);

    // Free the memory used by the reconstructed image
    free(reconstructed_image);

    // Now, load the image back for decoding
    int width_1, height_1, channels_1;
    const char *input_filename_1 = "output_image.png";  // Use the previously saved output image

    unsigned char *image_1 = stbi_load(input_filename_1, &width_1, &height_1, &channels_1, 0);
    if (image_1 == NULL) {  // Check if image loading was successful
        printf("Failed to load image\n");
        return 1;  // Return failure if loading fails
    }

    // Display the loaded image's dimensions and channels
    printf("Image loaded successfully! %d x %d with %d channels\n", width_1, height_1, channels_1);

    // Convert the newly loaded image back into binary data for decoding
    int binary_size_1;
    char *binary_data_1 = image_to_binary(image_1, width_1, height_1, channels_1, &binary_size_1);
    if (!binary_data_1) {  // If conversion fails, free the loaded image and return failure
        stbi_image_free(image_1);
        return 1;
    }
    
    // Free the image memory since it is no longer needed
    stbi_image_free(image_1);

    // Decode the binary data into the original message's binary form
    char *message_binary_1 = decode_image(binary_data_1, width_1, height_1, channels_1);
    if (!message_binary_1) {  // If decoding fails, free the binary data and return failure
        free(binary_data_1);
        return 1;
    }
    
    // Convert the decoded binary message back to ASCII text
    char *ascii_message_1 = binaryToAscii(message_binary_1);
    if (!ascii_message_1) {  // If the conversion fails, free the allocated memory and return failure
        free(binary_data_1);
        free(message_binary_1);
        return 1;
    }

    // Print the decoded ASCII message
    printf("Decoded ASCII message: %s\n", ascii_message_1);

    // Free the allocated memory for binary data, message binary, and decoded ASCII message
    free(binary_data_1);
    free(message_binary_1);
    free(ascii_message_1);

    // Return success (0) to indicate the program completed without errors
    return 0;
}