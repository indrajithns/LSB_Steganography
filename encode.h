/* 
 * Image Steganography Encoding Header
 * 
 * Description:
 * This header file contains definitions and function prototypes required for encoding
 * a secret file (such as a text file) into a BMP image using steganography techniques. 
 * The main method of encoding is to store the secret file data in the least significant bits (LSB)
 * of the image pixels. The file also handles encoding the file extension, size, and data, as well as 
 * storing a magic string for identifying the stego image.
*/


#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
*/

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;          // Source image file name
    FILE *fptr_src_image;           // File pointer for source image
    uint image_capacity;            // Capacity of the image to store secret data
    uint bits_per_pixel;            // Bits per pixel (for BMP images, it is 24)
    char image_data[MAX_IMAGE_BUF_SIZE]; // Buffer to store image data

    /* Secret File Info */
    char *secret_fname;             // Secret file name to encode
    FILE *fptr_secret;              // File pointer for secret file
    char extn_secret_file[MAX_FILE_SUFFIX]; // Extension of the secret file
    char secret_data[MAX_SECRET_BUF_SIZE];  // Buffer to store secret file data
    long size_secret_file;          // Size of the secret file

    /* Stego Image Info */
    char *stego_image_fname;        // Stego image file name (output image)
    FILE *fptr_stego_image;         // File pointer for stego image

} EncodeInfo;


/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* Check capacity of source image to store secret data */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size for BMP image */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header from source to stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String (for identifying stego image) */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extension size into the image */
Status encode_secret_file_extn_size(int file_extn_size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode secret file extension into the image */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size into the image */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding of data into image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo);

/* Encode a byte into the Least Significant Bit (LSB) of image data */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Encode a size into LSB of image data array */
Status encode_size_to_lsb(int size, char *image_buffer);

/* Copy remaining image bytes from source to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
