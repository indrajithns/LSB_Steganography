# LSB Image Steganography on BMP Files

## Introduction
This project implements **Least Significant Bit (LSB) Image Steganography** for embedding and extracting secret data within BMP image files. It provides functionalities for encoding a secret file into an image and decoding it back, using the BMP file format.

## Features
- **Encoding**: Embed secret text files into BMP images without visually altering the image.
- **Decoding**: Extract the secret text file from stego images.
- **BMP Header Preservation**: Ensures the BMP header remains unchanged for compatibility.
- **Magic String Identification**: Uses a unique magic string (`#*`) to verify encoded files.

## File Structure
- **Header Files:**
  - `encode.h`: Contains function prototypes and structures for encoding.
  - `decode.h`: Contains function prototypes and structures for decoding.
  - `types.h`: Defines custom types and enums for statuses and operations.
  - `common.h`: Contains shared constants and macros.

- **Source Files:**
  - `encode.c`: Implements the encoding process.
  - `decode.c`: Implements the decoding process.
  - `test_encode.c`: Driver program for testing encoding and decoding functionalities.

- **Key Structures:**
  - `EncodeInfo`: Stores data related to the encoding process.
  - `DecodeInfo`: Stores data related to the decoding process.

## Usage
### Compilation
Use `gcc` to compile the source files:
```bash
gcc -o steganography test_encode.c encode.c decode.c
```

### Running the Program
The program supports two operations: encoding and decoding.

#### Encoding
```bash
./steganography -e <source_image.bmp> <secret_file.txt> <stego_image.bmp>
```
- **Arguments:**
  - `source_image.bmp`: BMP image to encode data into.
  - `secret_file.txt`: Text file containing the data to be hidden.
  - `stego_image.bmp`: Output image file containing the hidden data.

#### Decoding
```bash
./steganography -d <stego_image.bmp> <output_secret_file.txt>
```
- **Arguments:**
  - `stego_image.bmp`: Image containing the hidden data.
  - `output_secret_file.txt`: Output text file to extract the hidden data.

### Example Commands
- **Encoding**:
  ```bash
  ./steganography -e beautiful.bmp secret.txt stego.bmp
  ```
- **Decoding**:
  ```bash
  ./steganography -d stego.bmp decoded_secret.txt
  ```

## Implementation Details
### Encoding Steps
1. **Validate Input:** Ensure files exist and are compatible (BMP format for the image, text format for the secret).
2. **Embed Metadata:**
   - Magic string (`#*`).
   - File extension and size of the secret file.
   - Secret file data.
3. **Preserve Remaining Image Data:** Copy the unmodified parts of the source image to the output stego image.

### Decoding Steps
1. **Validate Input:** Ensure the stego image is a valid BMP file.
2. **Extract Metadata:** Read the magic string, file extension, and size.
3. **Extract Data:** Retrieve the secret file content and write it to the output file.

### Key Functions
- **Encoding:**
  - `encode_magic_string`: Embeds the magic string.
  - `encode_data_to_image`: Encodes secret data into the image.
  - `copy_remaining_img_data`: Copies unmodified pixels from the source image.

- **Decoding:**
  - `decode_magic_string`: Verifies the magic string.
  - `decode_data_from_image`: Extracts secret data from the stego image.

## Limitations
- Works only with BMP file format.
- Secret file size must be small enough to fit within the available image capacity.

## Requirements
- **C Compiler:** GCC or any standard C compiler.
- **Input Files:**
  - BMP image as the source.
  - Text file as the secret.

