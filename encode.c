/*
documentation
Name:Indrajith
Date:24-08-2024
Project: LSB Image Steganography on .bmp file
documentation
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}



Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (strcmp(strstr(argv[2],"."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }

    else
    {
        return e_failure;
    }

    if (strcmp(strstr(argv[3],"."), ".txt") == 0)
    {
        encInfo->secret_fname = argv[3];
    }
    else
    {
        return e_failure;
    }

    if(argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    fseek(encInfo->fptr_secret, 0, SEEK_SET);

    if(encInfo->image_capacity >= 54 + 16 + 32 + 32 + 32 + (8 * encInfo->size_secret_file))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_SET);
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(buffer, sizeof(char), 54, fptr_src_image);
    fwrite(buffer, sizeof(char), 54, fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, 2, encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, sizeof(char), 8, fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, sizeof(char), 8, fptr_stego_image);
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned int mask = 0x80, i;
    for(i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
}

Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[32];
    fread (buffer, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, fptr_stego_image);
    return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
    unsigned int mask = 1 << 31, i;
    for(i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status encode_secret_file_size(long size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread (buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    char str[encInfo->size_secret_file];
    fread(str, encInfo->size_secret_file, 1, encInfo->fptr_secret);
    encode_data_to_image(str, strlen(str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while((fread(&ch, 1, 1, fptr_src)) > 0)
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("Started Encoding...\n");
    if (open_files(encInfo) == e_success)
    {
        printf("Opening of File Successful...\n");
        if(check_capacity(encInfo) == e_success)
        {
            printf("Check Capacity Successful...\n");
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("Copying Header Successful...\n");
                if(encode_magic_string(MAGIC_STRING, encInfo)== e_success)
                {
                    printf("Encoding of Maic String is Successful...\n");
                    strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname, "."));
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                    {

                        printf("Encoding of Secret File Extension Size is Successful...\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoding of Secret File Extension is Successful...\n");
                            if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Encoding of Secret File Size is Successful...\n");
                                if(encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoding of Secret File Data is Successful...\n");
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("Copying Remaining Image Data is Successful...\n");
                                    }
                                    else
                                    {
                                        printf("Copying Remaining Image Data is Failed...\n");
                                        return e_failure;
                                    }

                                }
                                else
                                {
                                    printf("Encoding of Secret File Data is Failed...\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Encoding of Secret File Size is Failed...\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Encoding of Secret File Extension is Failed...\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Encoding of Secret File Extension Size is Failed...\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Encoding of Maic String is Failed...\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Copying Header Failed...\n");
                return e_failure;
            }

        }
        else
        {
            printf("Check Capacity Failed...\n");
            return e_failure;
        }
    }
    else
    {
        printf("Opening of Files Failed...\n");
        return e_failure;
    }
    return e_success;
}
