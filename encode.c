/*
 * Image Steganography Encoding Functions
 * 
 * Description:
 * This code implements the encoding process for hiding a secret file (text or binary) inside a BMP image. 
 * The encoding includes several steps:
 * 1. Reading the source image and secret file.
 * 2. Verifying if the image has enough capacity to store the secret file.
 * 3. Modifying the least significant bits (LSB) of the image pixels to store the secret data.
 * 4. Saving the resulting stego image with the hidden data.
 * 
 * Key functionalities include:
 * - Extracting the image size and verifying available capacity.
 * - Copying the image header to the new stego image.
 * - Encoding the magic string, secret file extension, size, and content into the image.
 * - Copying the remaining image data after encoding.
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    printf("Width = %u\n", width);
    fread(&height, sizeof(int), 1, fptr_image);
    printf("Height = %u\n", height);
    return width * height * 3;
}

/* Open files for source image, secret file, and stego image */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL) return e_failure;

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL) return e_failure;

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL) return e_failure;

    return e_success;
}

/* Validate and read input arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
        encInfo->src_image_fname = argv[2];
    else return e_failure;

    if (strcmp(strstr(argv[3], "."), ".txt") == 0)
        encInfo->secret_fname = argv[3];
    else return e_failure;

    encInfo->stego_image_fname = argv[4] ? argv[4] : "stego.bmp";
    return e_success;
}

/* Check if the image has enough capacity to hold the secret file */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if (encInfo->image_capacity >= 54 + 16 + 32 + 32 + 32 + (8 * encInfo->size_secret_file))
        return e_success;
    return e_failure;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_SET);
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/* Copy BMP header from source to destination */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(buffer, sizeof(char), 54, fptr_src_image);
    fwrite(buffer, sizeof(char), 54, fptr_dest_image);
    return e_success;
}

/* Encode a magic string into the image */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/* Encode secret data byte by byte into image pixels */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, sizeof(char), 8, fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, sizeof(char), 8, fptr_stego_image);
    }
}

/* Encode a byte into the LSB of image data */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned int mask = 0x80, i;
    for (i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
}

/* Encode the secret file extension size */
Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];
    fread(buffer, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, fptr_stego_image);
    return e_success;
}

/* Encode the size of secret file in LSB */
Status encode_size_to_lsb(int size, char *image_buffer)
{
    unsigned int mask = 1 << 31, i;
    for (i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
    return e_success;
}

/* Encode the secret file extension into the image */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/* Encode secret file size into the image */
Status encode_secret_file_size(long size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread (buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

/* Encode the secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/* Copy the remaining image data after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while ((fread(&ch, 1, 1, fptr_src)) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

/* Perform the encoding process */
Status do_encoding(EncodeInfo *encInfo)
{
    printf("Encoding Started...\n");
    if (open_files(encInfo) == e_success)
    {
        printf("Opening of File Successful...\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("Capacity Check Successful...\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Header Copy Successful...\n");
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("Encoding of Magic String is Successful...\n");
                    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));
                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {
                        printf("Encoding of Secret File Extension Size is Successful...\n");
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoding of Secret File Size is Successful...\n");
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Encoding of Secret File Size Successful...\n");
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoding of Secret File Data Successful...\n");
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("Remaining Image Data Copy Successful...\n");
                                    }
                                    else
                                    {
                                        printf("Copying Remaining Image Data Failed...\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Encoding of Secret File Data Failed...\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Encoding of Secret File Size Failed...\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encoding of Secret File Extension Failed...\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Encoding of Secret File Extension Size Failed...\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Encoding of Magic String Failed...\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Header Copy Failed...\n");
                return e_failure;
            }
        }
        else
        {
            printf("Capacity Check Failed...\n");
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
