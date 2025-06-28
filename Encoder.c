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
 * Converts a given message into its binary representation.
 *
 * @param message The ASCII message to be converted into binary.
 * @return The binary representation of the message as a string.
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
 * This function is now modified to correctly embed individual bits of the message.
 *
 * @param binary_data The binary data to which the message is to be encoded.
 * @param message The message to be encoded in binary.
 * @return The binary string with the encoded message and checksum appended.
 */
char *encode_binary(char *binary_data, const char *message) {
    char *binary_message = messageToBinary(message);
    if (!binary_message) {
        return NULL;
    }

    // Convert checksum to binary string
    unsigned char checksum_val = calculate_checksum(binary_message, strlen(binary_message));
    char checksum_binary[9]; // 8 bits + null terminator
    for (int j = 0; j < 8; j++) {
        checksum_binary[j] = ((checksum_val >> (7 - j)) & 1) ? '1' : '0';
    }
    checksum_binary[8] = '\0';

    char end_marker_binary[] = "00000111"; // ASCII for BEL character (Bell)

    int message_bits_len = strlen(binary_message);
    int checksum_bits_len = strlen(checksum_binary);
    int end_marker_bits_len = strlen(end_marker_binary);

    int total_bits_to_embed = message_bits_len + checksum_bits_len + end_marker_bits_len;

    int original_image_binary_len = strlen(binary_data); // Total number of '0'/'1' characters for the image

    // Check if the image is large enough to embed the entire message + checksum + end marker
    // We modify 1 LSB per 8-bit component representation in binary_data.
    // So, we need at least `total_bits_to_embed` LSB positions.
    if (original_image_binary_len < (total_bits_to_embed * 8)) { // Must have enough `char`s in `binary_data`
        printf("Error: Image is too small to embed the message. Needs at least %d pixel LSBs (total %d image binary bits).\n", 
               total_bits_to_embed, total_bits_to_embed * 8);
        free(binary_message);
        return NULL;
    }

    int current_embed_idx = 0; // Index for bits in `binary_message` / `checksum_binary` / `end_marker_binary`
    int binary_data_lsb_idx = 7; // Index for LSB position in `binary_data` (0-7 for first byte, 8-15 for second, etc.)

    // Embed message bits
    for (int i = 0; i < message_bits_len; i++) {
        // Embed the actual bit from binary_message (e.g., '0' or '1')
        binary_data[binary_data_lsb_idx] = binary_message[current_embed_idx++];
        binary_data_lsb_idx += 8; // Move to the LSB of the next 8-bit component
    }

    // Embed checksum bits
    for (int i = 0; i < checksum_bits_len; i++) {
        binary_data[binary_data_lsb_idx] = checksum_binary[i];
        binary_data_lsb_idx += 8;
    }

    // Embed end marker bits
    for (int i = 0; i < end_marker_bits_len; i++) {
        binary_data[binary_data_lsb_idx] = end_marker_binary[i];
        binary_data_lsb_idx += 8;
    }
    
    free(binary_message);
    return binary_data; // Return the modified binary_data string
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
    // Store strlen(binary_data) once to avoid repeated calls in the loop (PERFORMANCE FIX)
    size_t binary_data_len = strlen(binary_data); 

    // Convert binary data back to image format
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                unsigned char pixel_value = 0;
                for (int i = 0; i < 8; i++) {
                    // Ensure we don't read beyond the allocated binary_data
                    if (binary_index >= binary_data_len) { 
                        printf("Warning: Ran out of binary data before filling image pixels. Image might be incomplete.\n");
                        free(image); 
                        return NULL;
                    }
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
    // Allocate memory for the potential decoded message (max possible bits = total LSBs in image data)
    char *result = (char *)malloc((length / 8) + 1); 
    
    if (!result) {
        printf("Memory allocation failed!\n");
        return NULL;
    }

    int result_index = 0;
    char byte[9];
    unsigned char extracted_checksum = 0;
    
    for (int i = 7; i < length; i += 8) {
        result[result_index++] = image[i];

        if ((result_index) % 8 == 0) {
            strncpy(byte, result + result_index - 8, 8);
            byte[8] = '\0';
            
            // Check for end marker in the binary data
            if (strcmp(byte, "00000111") == 0) {
                // End marker found.
                // The structure is: [MESSAGE BITS] [CHECKSUM BITS (8)] [END MARKER BITS (8)]
                // `result_index` now includes the 8 bits of the end marker.

                // Ensure enough bits for checksum (8) + end marker (8)
                if (result_index < 16) { 
                    printf("Error: End marker found, but not enough preceding data for checksum (requires 16 bits).\n");
                    free(result);
                    return NULL;
                }

                // Extract checksum (8 bits just before the end marker)
                strncpy(byte, result + result_index - 16, 8);
                byte[8] = '\0';
                extracted_checksum = 0;
                for (int k = 0; k < 8; k++) {
                    extracted_checksum = (extracted_checksum << 1) | (byte[k] - '0');
                }

                // The message bits are everything before the checksum
                int message_bit_length = result_index - 16;
                if (message_bit_length < 0) { 
                    // This scenario should ideally be caught by the result_index < 16 check above,
                    // but as a safeguard.
                    printf("Error: Negative message length calculated after extraction.\n");
                    free(result);
                    return NULL;
                }

                // Null-terminate the result string at the end of the actual message bits
                result[message_bit_length] = '\0';

                // Calculate checksum of the extracted message content (which is still in binary string format)
                unsigned char calculated_checksum = calculate_checksum(result, message_bit_length);
                
                // Verify the checksum
                if (calculated_checksum != extracted_checksum) {
                    printf("Warning: Checksum verification failed! Message may be corrupted.\n");
                    printf("Expected checksum: %02X, Got: %02X\n", extracted_checksum, calculated_checksum);
                } else {
                    printf("Checksum verification successful!\n");
                }
                
                return result; // Return the binary string of the message
            }
        }
    }

    // If end marker is NOT found after iterating through all possible LSBs
    printf("Error: End marker '00000111' not found in the image's LSBs.\n");
    free(result); // Free the allocated memory since no message was found
    return NULL;
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
    // Declare pointers for dynamically allocated memory, initialized to NULL for safety
    unsigned char *image = NULL;
    char *binary_image_data = NULL;
    unsigned char *reconstructed_image = NULL;
    char *decoded_binary_message = NULL;
    char *ascii_message = NULL;

    // Declare necessary integer variables for image properties
    int width, height, channels;
    int binary_size; // Declare binary_size here

    char input_filename_buffer[256];
    char output_filename_buffer[256];
    char choice_str[10];
    char message_to_encode[1024]; // Increased buffer for message

    printf("Welcome to the Image Steganography CLI!\n");
    printf("Press 'q' at any time to quit.\n\n"); // Display quit instruction once at the beginning

    while (1) { // Main program loop
        // Reset pointers for current iteration to ensure clean state and avoid double-free issues
        image = NULL;
        binary_image_data = NULL;
        reconstructed_image = NULL;
        decoded_binary_message = NULL;
        ascii_message = NULL;

        // --- Get input image filename ---
        printf("Enter the input image filename (e.g., images/IMAGEASCII.png): ");
        if (fgets(input_filename_buffer, sizeof(input_filename_buffer), stdin) == NULL) {
            printf("ERROR: Failed to read input filename.\n");
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
        }
        input_filename_buffer[strcspn(input_filename_buffer, "\n")] = 0; // Remove trailing newline

        if (strcmp(input_filename_buffer, "q") == 0 || strcmp(input_filename_buffer, "Q") == 0) {
            printf("Quitting program.\n");
            goto full_program_exit; // Exit the while loop
        }

        // --- Ask user to choose encode/decode ---
        printf("\nDo you want to (1) Encode or (2) Decode a message? (Enter 1 or 2): ");
        if (fgets(choice_str, sizeof(choice_str), stdin) == NULL) {
            printf("ERROR: Failed to read choice.\n");
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
        }
        choice_str[strcspn(choice_str, "\n")] = 0; // Remove trailing newline

        if (strcmp(choice_str, "q") == 0 || strcmp(choice_str, "Q") == 0) {
            printf("Quitting program.\n");
            goto full_program_exit; // Exit the while loop
        }
        int choice = atoi(choice_str);
        printf("\n"); // Add newline for spacing

        // --- Load image ---
        image = stbi_load(input_filename_buffer, &width, &height, &channels, 0);
        if (image == NULL) {
            printf("ERROR: Failed to load image '%s'. Please ensure the file exists and is accessible.\n", input_filename_buffer);
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
        }
        printf("Image '%s' loaded successfully! Dimensions: %d x %d, Channels: %d\n", input_filename_buffer, width, height, channels);

        // --- Convert image to binary data ---
        binary_image_data = image_to_binary(image, width, height, channels, &binary_size);
        stbi_image_free(image); // Free original image data
        image = NULL; // Set to NULL after freeing
        if (!binary_image_data) {
            printf("ERROR: Failed to convert image to binary data.\n");
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
        }

        if (choice == 1) { // Encode path
            // Calculate max character length based on the image's binary data
            int total_lsb_positions = strlen(binary_image_data) / 8; 
            int overhead_bits = 8 + 8; // 8 bits for checksum, 8 bits for end marker
            int available_message_bits = total_lsb_positions - overhead_bits;
            int max_char_length = available_message_bits / 8; // Each char is 8 bits

            if (max_char_length <= 0) {
                printf("ERROR: The image is too small to encode any message. (Needs at least %d LSB positions for overhead, has %d available).\n", overhead_bits, total_lsb_positions);
                goto cleanup_iteration_and_continue;
            }
            
            printf("--- Encoding Mode ---\n"); // Section header
            printf("Maximum message length: %d characters.\n", max_char_length); // Clearer label
            
            while (1) { // Loop for message input
                printf("Enter the message you want to encode: ");
                if (fgets(message_to_encode, sizeof(message_to_encode), stdin) == NULL) {
                    printf("ERROR: Failed to read message.\n");
                    goto cleanup_iteration_and_continue;
                }
                message_to_encode[strcspn(message_to_encode, "\n")] = 0; // Remove trailing newline

                if (strcmp(message_to_encode, "q") == 0 || strcmp(message_to_encode, "Q") == 0) {
                    printf("Quitting program.\n");
                    goto full_program_exit;
                }

                if (strlen(message_to_encode) * 8 > available_message_bits) {
                    printf("WARNING: Message is too long! Please enter a message up to %d characters.\n", max_char_length);
                } else {
                    break; // Valid message, exit inner loop
                }
            }

            printf("Enter the output filename for the encoded image (e.g., output_encoded.png): ");
            if (fgets(output_filename_buffer, sizeof(output_filename_buffer), stdin) == NULL) {
                printf("ERROR: Failed to read output filename.\n");
                goto cleanup_iteration_and_continue;
            }
            output_filename_buffer[strcspn(output_filename_buffer, "\n")] = 0; // Remove trailing newline

            if (strcmp(output_filename_buffer, "q") == 0 || strcmp(output_filename_buffer, "Q") == 0) {
                printf("Quitting program.\n");
                goto full_program_exit;
            }

            // Call encode_binary with the original binary_data and user's message
            char *encoded_binary_data = encode_binary(binary_image_data, message_to_encode); 
            if (!encoded_binary_data) {
                printf("ERROR: Failed to encode message into binary data.\n");
                goto cleanup_iteration_and_continue;
            }
            // binary_image_data now holds the encoded data.

            // Reconstruct the image from the modified binary data
            reconstructed_image = binary_to_image(encoded_binary_data, width, height, channels);
            if (!reconstructed_image) {
                printf("ERROR: Failed to reconstruct image from binary data.\n");
                goto cleanup_iteration_and_continue;
            }

            // Save the reconstructed image
            if (!stbi_write_png(output_filename_buffer, width, height, channels, reconstructed_image, width * channels)) {
                printf("ERROR: Failed to write encoded image to '%s'. Ensure you have write permissions.\n", output_filename_buffer);
                goto cleanup_iteration_and_continue;
            }

            printf("\nSUCCESS: Message successfully encoded and image saved to '%s'\n", output_filename_buffer);
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop

        } else if (choice == 2) { // Decode
            printf("--- Decoding Mode ---\n"); // Section header
            printf("Attempting to decode message...\n");
            // Call decode_image to extract the binary message
            decoded_binary_message = decode_image(binary_image_data, width, height, channels);
            
            // binary_image_data is consumed by decode_image logic, so free it here.
            // If decode_image returns NULL, it has already freed its internal 'result' memory.
            if (binary_image_data) free(binary_image_data); 
            binary_image_data = NULL; // Set to NULL after freeing

            if (!decoded_binary_message) {
                printf("RESULT: No message found encoded in this image or decoding failed (e.g., end marker not found, corrupted data).\n");
                goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
            }
            
            // Convert the decoded binary message to ASCII
            ascii_message = binaryToAscii(decoded_binary_message);
            if (!ascii_message) {
                printf("ERROR: Failed to convert decoded binary message to ASCII.\n");
                goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
            }

            printf("Decoded message: \"%s\"\n", ascii_message);
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop

        } else { // Invalid choice
            printf("ERROR: Invalid choice. Please enter '1' for Encode or '2' for Decode.\n");
            goto cleanup_iteration_and_continue; // Go to cleanup and continue loop
        }

    cleanup_iteration_and_continue:
        // Centralized cleanup for memory allocated within this specific iteration
        // These checks are crucial because some pointers might be NULL already if freed earlier or due to error paths.
        if (image) free(image); 
        if (binary_image_data) free(binary_image_data); // Will be freed here if not freed in decode path
        if (reconstructed_image) free(reconstructed_image);
        if (decoded_binary_message) free(decoded_binary_message);
        if (ascii_message) free(ascii_message);
        printf("\n----------------------------------------\n\n"); // Separator for next iteration
        continue; // Continue to the next iteration of the main loop

    full_program_exit:
        // Final cleanup before exiting the entire program
        // This handles cases where 'q' was pressed and some pointers might still hold data.
        if (image) free(image);
        if (binary_image_data) free(binary_image_data);
        if (reconstructed_image) free(reconstructed_image);
        if (decoded_binary_message) free(decoded_binary_message);
        if (ascii_message) free(ascii_message);
        return 0; // Exit the program gracefully
    }
}
