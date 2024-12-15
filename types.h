/*
 * Custom Types and Enumerations for Steganography Program
 * 
 * Description:
 * This header file defines custom data types and enumerations used across 
 * the steganography program. It provides:
 * - A type alias `uint` for unsigned integers.
 * - A `Status` enumeration to represent success or failure of operations.
 * - An `OperationType` enumeration to differentiate between encoding, 
 *   decoding, and unsupported operations.
*/

#ifndef TYPES_H
#define TYPES_H

/* User-defined type for unsigned integers */
typedef unsigned int uint;

/* Enumeration for function return status */
typedef enum
{
    e_success,   // Indicates successful operation
    e_failure    // Indicates failed operation
} Status;

/* Enumeration for operation type */
typedef enum
{
    e_encode,      // Encoding operation
    e_decode,      // Decoding operation
    e_unsupported  // Unsupported operation
} OperationType;

#endif
