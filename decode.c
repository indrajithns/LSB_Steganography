/*
 * Image Decoding Utility
 * 
 * Description:
 * This program decodes hidden data (such as a secret file) from a BMP image.
 * It performs the following tasks:
 * - Validates input arguments
 * - Opens and reads the image file
 * - Decodes a magic string to verify data presence
 * - Extracts secret file extension and size
 * - Decodes the hidden secret file data and saves it to a file
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Validate decoding arguments and set file names
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the source image file is a .bmp file
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->d_src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // Set the secret file name, defaulting to "decode.txt" if not provided
    if (argv[3] != NULL)
    {
        decInfo->d_secret_fname = argv[3];
    }
    else
    {
        decInfo->d_secret_fname = "decode.txt";
    }
    return e_success;
}

// Open the source image file for reading
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_d_src_image = fopen(decInfo->d_src_image_fname, "r");
    if (decInfo->fptr_d_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to Open The File %s\n", decInfo->d_src_image_fname);
        return e_failure;
    }
    return e_success;
}

// Decode the magic string from the image to validate data presence
Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_d_src_image, 54L, SEEK_SET); // Skip BMP header
    decInfo->magic_data = malloc(strlen(MAGIC_STRING) + 1);

    decode_data_from_image(strlen(MAGIC_STRING), decInfo->fptr_d_src_image, decInfo);

    // Check if the decoded magic string matches the expected string
    if (strcmp(decInfo->magic_data, MAGIC_STRING) == 0)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Decode data of specified size from the image
Status decode_data_from_image(int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    char str[8];
    for (int i = 0; i < size; i++)
    {
        fread(str, 8, 1, fptr_d_src_image); // Read 8 bits
        decode_byte_from_lsb(&decInfo->magic_data[i], str); // Decode the byte
    }
    decInfo->magic_data[size] = '\0';
    return e_success;
}

// Decode a byte from the least significant bits (LSBs) of the buffer
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    unsigned char ch = 0x00;
    int shift = 7;
    for (int i = 0; i < 8; i++)
    {
        ch = ((image_buffer[i] & 0x01) << shift--) | ch; // Extract LSBs
    }
    *data = ch;
    return e_success;
}

// Decode and validate the size of the file extension
Status decode_extension_size(int size, FILE *fptr_d_src_image)
{
    char str[32];
    int length;

    fread(str, 32, sizeof(char), fptr_d_src_image); // Read 32 bits
    decode_size_from_lsb(str, &length);

    return (length == size) ? e_success : e_failure;
}

// Decode size from the least significant bits
Status decode_size_from_lsb(char *buffer, int *size)
{
    int num = 0x00;
    int j = 31;
    for (int i = 0; i < 32; i++)
    {
        num = ((buffer[i] & 0x01) << j--) | num; // Extract 32 bits
    }
    *size = num;
}

// Decode the secret file extension from the image
Status decode_secret_file_extension(char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt"; // Expected extension
    int size = strlen(file_ext);
    decInfo->d_extn_secret_file = malloc(size + 1);

    decode_extension_data(size, decInfo->fptr_d_src_image, decInfo);

    decInfo->d_extn_secret_file[size] = '\0';
    return (strcmp(decInfo->d_extn_secret_file, file_ext) == 0) ? e_success : e_failure;
}

// Decode file extension data
Status decode_extension_data(int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(decInfo->d_src_image_fname, 8, 1, fptr_d_src_image); // Read 8 bits
        decode_byte_from_lsb(&decInfo->d_extn_secret_file[i], decInfo->d_src_image_fname);
    }
    return e_success;
}

// Decode the size of the secret file
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo)
{
    char str[32];
    fread(str, 32, sizeof(char), decInfo->fptr_d_src_image); // Read 32 bits
    decode_size_from_lsb(str, &file_size);
    decInfo->size_secret_file = file_size;

    return e_success;
}

// Decode the secret file data and write it to a file
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    decInfo->fptr_d_secret = fopen("decode_secret_data.txt", "w");
    if (decInfo->fptr_d_secret == NULL)
    {
        fprintf(stderr, "Can't Open decode.txt file\n");
        return e_failure;
    }

    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(decInfo->d_src_image_fname, 8, sizeof(char), decInfo->fptr_d_src_image);
        decode_byte_from_lsb(&ch, decInfo->d_src_image_fname);
        fputc(ch, decInfo->fptr_d_secret); // Write decoded data
    }
    return e_success;
}

// Perform the entire decoding process step-by-step
Status do_decoding(DecodeInfo *decInfo)
{
    printf("Decoding Started...\n");
    if (open_files_decode(decInfo) == e_success)
    {
        printf("Opening of File Successful...\n");
        if (decode_magic_string(decInfo) == e_success)
        {
            printf("Decoding of Magic String is Successful...\n");
            if (decode_extension_size(strlen(".txt"), decInfo->fptr_d_src_image) == e_success)
            {
                printf("Decoding of Secret File Extension Size is Successful...\n");
                if (decode_secret_file_extension(decInfo->d_extn_secret_file, decInfo) == e_success)
                {
                    printf("Decoding of Secret File Extension is Successful...\n");
                    if (decode_secret_file_size(decInfo->size_secret_file, decInfo) == e_success)
                    {
                        printf("Decoding of Secret File Size is Successful...\n");
                        if (decode_secret_file_data(decInfo) == e_success)
                        {
                            printf("Decoding of Secret File Data is Successful...\n");
                        }
                        else
                        {
                            printf("Decoding of Secret File Data Failed...\n");
                        }
                    }
                    else
                    {
                        printf("Decoding of Secret File Size Failed...\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Decoding of Secret File Extension Failed...\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Decoding of Secret File Extension Size Failed...\n");
                return e_failure;
            }
        }
        else
        {
            printf("Decoding of Magic String Failed...\n");
            return e_failure;
        }
    }
    else
    {
        printf("Opening of File Failed...\n");
        return e_failure;
    }
    return e_success;
}
