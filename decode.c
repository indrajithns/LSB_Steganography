/*
documentation
Name:Indrajith
Date:24-08-2024
Project: LSB Image Steganography on .bmp file
documentation
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strstr(argv[2], "."),".bmp") == 0)
    {
        decInfo->d_src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }
    if(argv[3] != NULL)
    {
        decInfo->d_secret_fname=argv[3];
    }
    else
    {
        decInfo->d_secret_fname = "decode.txt";
    }
    return e_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_d_src_image = fopen(decInfo->d_src_image_fname, "r");

    if(decInfo->fptr_d_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR:Unable to open the file %s\n", decInfo->d_src_image_fname);
        return e_failure;
    }
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{

    fseek(decInfo->fptr_d_src_image, 54L, SEEK_SET);
    int i = strlen(MAGIC_STRING);
    decInfo -> magic_data = malloc(strlen(MAGIC_STRING)+1);

    decode_data_from_image(strlen(MAGIC_STRING), decInfo->fptr_d_src_image, decInfo );

    if(strcmp (decInfo->magic_data , MAGIC_STRING ) == 0 )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_data_from_image ( int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    int i;
    char str[8];
    for (i = 0; i < size; i++)
    {
        fread (str, 8, 1, fptr_d_src_image);

        decode_byte_from_lsb (&decInfo->magic_data[i], str);
    }
    decInfo -> magic_data[i] ='\0';
    return e_success;
}

Status decode_byte_from_lsb (char *data, char *image_buffer)
{
    int shift = 7;
    unsigned char ch =0x00;
    for (int i = 0; i < 8; i++)
    {
        ch = ( (image_buffer[i] & 0x01 ) << shift--) | ch;
    }
    *data = ch;
    return e_success;
}

Status decode_extension_size (int size, FILE *fptr_d_src_image)
{
    char str[32];
    int length;

    fread (str, 32, sizeof(char), fptr_d_src_image);
    decode_size_from_lsb(str,&length) ;

    if (length == size )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_size_from_lsb (char *buffer, int *size)
{
    int j = 31;
    int num = 0x00;

    for (int i = 0; i < 32; i++)
    {
        num = ( (buffer[i] & 0x01 ) << j-- ) | num ;
    }
    *size = num;

}

Status decode_extension_data ( int size, FILE *fptr_d_src_image, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {

        fread (decInfo->d_src_image_fname, 8, 1, fptr_d_src_image);

        decode_byte_from_lsb (&decInfo->d_extn_secret_file[i], decInfo->d_src_image_fname);
    }
    return e_success;
}

Status decode_secret_file_extension (char *file_ext, DecodeInfo *decInfo)
{
    file_ext = ".txt";
    int i = strlen(file_ext);
    decInfo->d_extn_secret_file = malloc (i + 1);

    decode_extension_data(strlen(file_ext), decInfo->fptr_d_src_image, decInfo);

    decInfo->d_extn_secret_file[i] = '\0';

    if (  strcmp (decInfo->d_extn_secret_file , file_ext ) == 0 )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status decode_secret_file_size (int file_size, DecodeInfo *decInfo)
{

    char str[32];

    fread (str, 32, sizeof(char), decInfo->fptr_d_src_image);
    decode_size_from_lsb(str,&file_size) ;
    decInfo->size_secret_file = file_size;

    return e_success;
}

Status decode_secret_file_data (DecodeInfo *decInfo)
{
    char ch;
    decInfo->fptr_d_secret = fopen("decode_secret_data.txt","w");

    if ( decInfo->fptr_d_secret == NULL )
    {
        fprintf(stderr,"Can't open decode.txt file\n");
        return e_failure;
    }
    else
    {
        for (int i=0; i < decInfo->size_secret_file; i++)
        {
            fread (decInfo->d_src_image_fname, 8 , sizeof(char), decInfo->fptr_d_src_image);

            decode_byte_from_lsb (&ch, decInfo->d_src_image_fname);

            fputc (ch, decInfo->fptr_d_secret);
        }
        return e_success;
    }
}

Status do_decoding (DecodeInfo *decInfo)
{
    printf("Started Decoding...\n");
    if (open_files_decode(decInfo) == e_success)
    {
        printf("Opening of File Successful...\n");
        if (decode_magic_string(decInfo) == e_success)
        {
            printf("Decoding of Magic String is Successful...\n");

            if (decode_extension_size (strlen(".txt"), decInfo->fptr_d_src_image )  == e_success)
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
                            printf("Decoding Of Secret File Data is Failed\n");
                        }
                    }
                    else
                    {
                        printf("Decoding of Secret File Size is Failed...\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Decoding of Secret File Extension is Failed...\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Decoding of Secret File Extension Size is Failed...\n");
                return e_failure;
            }
        }
        else
        {
            printf("Decoding of Maic String is Failed...\n");
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
