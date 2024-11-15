#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)


typedef struct _DecodeInfo
{
    /* Source Image info */
    char *d_src_image_fname;
    FILE *fptr_d_src_image;
    char d_image_data[MAX_IMAGE_BUF_SIZE];
    char *magic_data;
    char *d_extn_secret_file;

    /* Secret File Info */
    int size_secret_file;
    FILE *fptr_d_dest_image;
    FILE *fptr_d_secret;
    char *d_secret_fname;
} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode a Data from Image */
Status decode_data_from_image(int size, FILE *fptr_d_src_image, DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_from_lsb (char *data, char *image_buffer);

/* Decode secret file extenstion size */
Status decode_extension_size (int size, FILE *fptr_d_src_image);

/* Decode LSB Size */
Status decode_size_from_lsb (char *buffer, int *size);

/* Decode secret file extenstion */
Status decode_secret_file_extension (char *file_ext, DecodeInfo *decInfo) ;

/* Decode secret file extenstion data */
Status decode_extension_data ( int size, FILE *fptr_d_src_image, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size (int file_size, DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data (DecodeInfo *decInfo);

#endif