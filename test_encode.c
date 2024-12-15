/*
 * Main Program for Image Steganography
 *
 * Description:
 * This program allows the user to encode and decode secret files (text or binary)
 * within a BMP image using steganography techniques. It takes command-line arguments 
 * to select either encoding or decoding operations. The program supports the following:
 * - Encoding a secret file into a BMP image (hiding the secret data in the image pixels).
 * - Decoding a secret file from a BMP image (retrieving the hidden data from the image).
 * 
 * The operations are controlled by command-line options:
 * - Encoding: ./a.out -e source_image.bmp secret_file.txt stego_image.bmp
 * - Decoding: ./a.out -d stego_image.bmp decoded_file.txt
 *
 * The program will validate the arguments and proceed with the appropriate operation 
 * (encoding or decoding). If the arguments are invalid or insufficient, the program
 * will print an error message and guide the user with the correct usage.
*/

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    // Check if sufficient arguments are passed
    if(argc >= 4)
    {
        // Determine the operation type (encoding or decoding)
        if (check_operation_type(argv) == e_encode)
        {
            printf("Selected Encoding\n");
            EncodeInfo encInfo;

            // Validate encoding arguments
            if(read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("Successful Reading and Validating\n");

                // Perform the encoding process
                if(do_encoding(&encInfo) == e_success)
                {
                    printf("Encoding Successful\n");
                }
                else
                {
                    printf("Encoding Failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Failed Reading and Validating\n");
                return e_failure;
            }
        }
        else if(check_operation_type(argv) == e_decode)
        {
            printf("Selected Decoding\n");
            DecodeInfo decInfo;

            // Validate decoding arguments
            if(read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("Successful Reading and Validating\n");

                // Perform the decoding process
                if(do_decoding(&decInfo) == e_success)
                {
                    printf("Decoding Successful\n");
                }
                else
                {
                    printf("Decoding Failed\n");
                }
            }
            else
            {
                printf("Failed Reading and Validating\n");
                return e_failure;
            }
        }
        else
        {
            // Handle invalid operation type
            printf("Invalid Option\n");
            printf("---------------------------------Options---------------------------------\n");
            printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
            printf("Decoding: ./a.out -d stego.bmp decode.txt\n");
            printf("-------------------------------------------------------------------------\n");
        }
    }
    else
    {
        // Handle insufficient arguments
        printf("Error: Please Pass Minimum 4 Arguments\n");
    }

    return 0;
}

// Function to determine the operation type based on the command-line argument
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
