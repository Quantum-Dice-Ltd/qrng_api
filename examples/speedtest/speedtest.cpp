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
#include <string>
#include <thread>
#include <vector>
#include <iomanip>
#include <iostream>

#include "qrng_api.h" 

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)

#define DEBUG_PRINT printf

/**
* This function prints out the initial portion of an array of unsigned char 
* defined by the 'x' and 'y' arguments
* 
* @param[in]    data    The unsigned char array 
* @param[in]    size    size of the array 
* @param[in]    x, y    length and height of the matrix
*/
void debug_data_stream(unsigned char* data, int size, int x, int y)
{
    DEBUG_PRINT("\n\t Print data, size: %d, row: %d, col: %d", size, x, y);

    for (int i = 0; i < size; i++) {
        if ((i % x) == 0) DEBUG_PRINT("\n\t");
        if (i >= y * x) break;
        DEBUG_PRINT(" %02x", data[i]);
    }
    DEBUG_PRINT("\n");
}
 

int status_service(std::string thread_name, int run_count) {
	DEBUG_PRINT("\n Running speed test, reps: %d %s, Thread name: %s",
			run_count, (run_count == 0) ? "(infinite loop)" : " ",
			thread_name.c_str());

	std::vector<uint8_t> test_buf(MB(80));

	/**  Define second function */
	/** Initialize the QRNG api */
	#ifdef __unix__
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "/dev/xdma0" });
	#else _WIN32
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "0" });
	#endif
	DEBUG_PRINT("\n QRNG init function status: %d", qrng_get_status(qrng));

	if (qrng_get_status(qrng) != QRNG_SUCCESS) {
		DEBUG_PRINT("\n QRNG experienced ERROR [%d] during initialization",
			qrng_get_status(qrng));
		goto exit;
	}

	
	for (int i = 0; i < run_count || run_count == 0; i++) {
		//get speed

		auto start = std::chrono::high_resolution_clock::now();
		s32 bytes_count = 0;
		int ret = qrng_get(qrng, test_buf.data(), test_buf.size(), &bytes_count);
		auto end = std::chrono::high_resolution_clock::now();

		// Calculating total time taken by the program.
		double time_taken_ns = std::chrono::duration_cast<
				std::chrono::nanoseconds>(end - start).count();
		//std::cout << std::setprecision(3) << std::fixed;
		//std::cout << time_taken_ns / pow(10,6) << "ms, " << bytes_count << "bytes, ";
		std::cout << "[" << std::setw(6) << i << "]\t" << thread_name
				<< ", st: " << ret << ", " << std::setprecision(3)
				<< ((test_buf.size() / time_taken_ns) * 8) << "gbps\n";

		//sleep
#ifdef _WIN32
			Sleep(2000);
#else
		sleep(2);
#endif
	}

	exit:
		qrng_deinit(qrng); 
		return 0;
}

void print_head() {
	/* Print headers for better presentation */
	std::cout << "\nStatus descrition: \n";
	std::cout << -9 << " Internal buffer error \n";
	std::cout << -8 << " Internal initialization error: insufficient memory\n";
	std::cout << -7 << " Internal error reading device\n";
	std::cout << -6 << " Incomplete data was received \n";
	std::cout << -5
			<< " Unable to initialize device, could be driver issues or insufficient priviledges\n";
	std::cout << -4 << " QRNG is not initialized \n";
	std::cout << -3 << " QRNG is already initialized \n";
	std::cout << -2 << " Null pointer error\n";
	std::cout << -1
			<< " No device found, could also be absense of driver or disconnected hardware\n";
	std::cout << 0 << " No error \n\n";

	std::cout << std::setw(30) << "QRNG (rand)" << std::setw(30)
			<< " QRNG (get)" << "\n";

}

/** Main programs */
int multithread(int argc, char **argv) {
	int run_count = (argc > 1) ? atoi(argv[1]) : 0; //default run for multithread should be 2
	int num_of_threads = 1;
	print_head();

	std::vector<std::thread> threads(num_of_threads);
	for (int i = 0; i < num_of_threads; i++) {
		threads[i] = std::thread(status_service, "Thread" + std::to_string(i),
				run_count);
	}

	for (int i = 0; i < num_of_threads; i++) {
		threads[i].join();
	}

	return 0;
}

int main(int argc, char **argv)
{
	return multithread(argc, argv);
}

