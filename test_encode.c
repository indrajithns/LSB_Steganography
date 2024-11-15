/*
documentation
Name:Indrajith
Date:24-08-2024
Project: LSB Image Steganography on .bmp file
documentation
*/


#include <stdio.h>
#include <string.h>
#include "common.h"
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    if(argc >= 4)
    {
        if (check_operation_type(argv) == e_encode)
        {
            printf("Selected Encoding\n");
            EncodeInfo encInfo;
            if(read_and_validate_encode_args(argv, &encInfo) == e_success)
            {
                printf("Successful Reading and Validating\n");
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
            if(read_and_validate_decode_args(argv, &decInfo) == e_success)
            {
                printf("Successful Reading and Validating\n");
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
            printf("Invalid Option\n");
            printf("---------------------------------Options---------------------------------\n");
            printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
            printf("Decoding: ./a.out -d stego.bmp decode.txt\n");
            printf("-------------------------------------------------------------------------\n");
        }
    }
    else
    {
        printf("Error: Please Pass Minimum 4 Argument\n");
    }
    return 0;

}


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

