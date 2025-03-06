/**
* @file     simple.cpp
* @brief    Sample program for generating random numbers with Quantum Dice QRNG.
* @author   Clifford Olawaiye
* @date     03/01/2023
*
* Copyright(c) Quantum Dice. All rights reserved.
*/

#include <stdio.h>
#include <vector>
#include <cstdint>
#include "qrng_api.h"
#include <cstdlib>

#define DEBUG_PRINT printf
#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)

void print_data_stream(unsigned char* data, int size, int x, int y);

int main()
{
    std::vector<uint8_t> test_data(MB(100));
    s32 bytes_read = 0;

    /** Initialize the QRNG api */
    #ifdef __unix__
        QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "/dev/xdma0" });
    #else //_WIN32
        QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "0" });
    #endif
    DEBUG_PRINT("\n QRNG init function status: %d", qrng_get_status(qrng));

    /** Getting a random integer (no need for seed) */
    int ret_int = qrng_rand(qrng);
    DEBUG_PRINT("\n QRNG rand function returned: %d, status: %d ", 
        ret_int, 
        qrng_get_status(qrng));

    /** Getting a stream of random data */
    qrng_get(qrng, test_data.data(), test_data.size(), &bytes_read);
    DEBUG_PRINT("\n QRNG get function returned: %d", qrng_get_status(qrng));
    print_data_stream((unsigned char*)test_data.data(), test_data.size(), 16, 10);

    /** Getting a zero to one uniform distribution */
    double ret_double = qrng_urand(qrng);
    DEBUG_PRINT("\n QRNG urand function returned: %f, status: %d", 
        ret_double, 
        qrng_get_status(qrng));

    qrng_deinit(qrng);
    return 0;
}


/**
* This function prints out the initial portion of an array of unsigned char
* defined by the 'x' and 'y' arguments
*
* @param[in]    data    The unsigned char array
* @param[in]    size    size of the array
* @param[in]    x, y    length and height of the matrix
*/
void print_data_stream(unsigned char* data, int size, int x, int y)
{
    DEBUG_PRINT("\n\t Print data, size: %d, row: %d, col: %d", size, x, y);

    for (int i = 0; i < size; i++) {
        if ((i % x) == 0) DEBUG_PRINT("\n\t");
        if (i >= y * x) break;
        DEBUG_PRINT(" %02x", data[i]);
    }
}
