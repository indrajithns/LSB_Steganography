Image Steganography

This program implements Image Steganography, a technique for hiding secret data (text or binary files) within an image, specifically a BMP image. The hidden data is encoded in the Least Significant Bits (LSB) of the image pixels, making it imperceptible to the human eye while preserving the integrity of the image.

Key Features:
Encoding: Hide a secret file (such as a text file) inside a BMP image.
Decoding: Retrieve the hidden secret file from a BMP image.
Magic String: A unique identifier is embedded in the image to mark it as a stego image.
File Extension and Size Encoding: The program encodes not only the secret data but also its extension and size within the image.
This tool can be useful for secure communication and data storage, as it allows for hiding sensitive information within seemingly innocent image files.

Workflow:
Encoding: The program reads the source BMP image, checks if it has enough capacity, encodes the secret file's data, and generates a new BMP image with the hidden data.
Decoding: The program reads the stego BMP image, extracts the secret data, and saves it to a new file.


1. Compile the Program

>> gcc -o steganography test_encode.c encode.c -lm

2. Encode a Secret File
To encode a secret file into a BMP image:

>> ./steganography -e <source_image.bmp> <secret_file.txt> <stego_image.bmp>

<source_image.bmp>: The BMP image to hide the secret data in.
<secret_file.txt>: The file containing the secret data.
<stego_image.bmp>: The output BMP image with hidden data.

3. Decode a Secret File
To decode a secret file from a BMP image:

>> ./steganography -d <stego_image.bmp> <decoded_file.txt>

<stego_image.bmp>: The BMP image containing the hidden data.
<decoded_file.txt>: The file where the extracted secret data will be saved.
