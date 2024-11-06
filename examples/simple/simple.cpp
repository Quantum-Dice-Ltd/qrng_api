/**
* @file     simple.cpp
* @brief    Sample program for generating random numbers with Quantum Dice QRNG.
* @author   Clifford Olawaiye
* @date     03/01/2023
*
* Copyright(c) Quantum Dice. All rights reserved.
*/

#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include "qrng_api.h" 

#define DEBUG_PRINT printf
#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
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
    DEBUG_PRINT("\n");
}
 

int main()
{
    std::vector<uint8_t> test_data(MB(100));
    s32 bytes_read = 0;

    /** Initialize the QRNG api */
    #ifdef __unix__
        QRNG *qrng = qrng_init_param({QRNG_VERTEX_B1, "/dev/xdma0"});
    #else _WIN32
        QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "0" });
    #endif
    DEBUG_PRINT("\n QRNG init function status: %d", qrng_get_status(qrng));

    if (qrng_get_status(qrng) != QRNG_SUCCESS) {
        DEBUG_PRINT("\n QRNG experienced ERROR [%d] during initialization", 
            qrng_get_status(qrng));
        goto exit;
    }

    /** Get 100 MB of random data */

    qrng_get(qrng, test_data.data(), test_data.size(), &bytes_read);
    
    if (qrng_get_status(qrng) != QRNG_SUCCESS) {
        DEBUG_PRINT("\n QRNG experienced ERROR [%x] while trying to fetch data", qrng_get_status(qrng));
        DEBUG_PRINT("\n The data returned is NOT a Quantum random number");
    }

    DEBUG_PRINT("\n QRNG get function returned: %d", qrng_get_status(qrng));
    print_data_stream((unsigned char*)test_data.data(), test_data.size(), 16, 10);

    exit: 
        qrng_deinit(qrng);
        return 0;
}

