/**
* @file     filedump.cpp
* @brief    Sample program for generating quantum random numbers with Quantum Dice QRNG.
* @author   Clifford Olawaiye
* @date     03/01/2023
*
* Copyright(c) Quantum Dice. All rights reserved.
* 
* This
*/

#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>  
#include <chrono>
#include <cstring>

#include <qrng_api.h>
#include <random>

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)

#define QRNG_RAW_DATA 1
#define QRNG_UDIST_DATA 2
#define PRNG_UDIST_DATA 0
#define PRNG_RAW_DATA 3

void print_help_info(){
	std::cout << "----------------------------------------------------------------------\n";
	std::cout << "How to run the program: \n";
	std::cout << "\t ./filedump filename filesize filecount datatype[qrng_raw/qrng_udist/prng]\n\n";
	std::cout << "e.g.\t ./filedump test_data 1024 5 qrng_raw\n";
	std::cout << "\t\t This sample command would dump 1kb (1024)\n";
	std::cout << "\t\t of 8 bit qrng data into 5 files prefixed with 'test_data' \n";
	std::cout << "\n NB:";
	std::cout << "\t qrng_raw: 8 bit qrng raw data\n";
	std::cout << "\t qrng_udist: uniform distribution between 0 and 1\n";
	std::cout << "\t prng_udist: uniform distribution with pseudo random numbers \n";
	std::cout << "\t prng_raw: raw pseudo random numbers \n";
	std::cout << "\n\t The filesize is capped at 10000000, this can be changed in the code \n";
	std::cout << "----------------------------------------------------------------------\n\n";
	//std::cout << "int size: " << sizeof(int64_t);
	
}


std::mt19937 mt{ std::random_device{}() };
std::uniform_real_distribution<> dist(0, 1.0);

int prng_init(){
	return 0;
}

int prng_get(u8* data, u32 data_len, s32* read_len){
	for (u32 i = 0; i < data_len; i++) {
		data[i] = mt() * 0xff;
//		data[i] = (mt() * 255 / 0xffffffff);
	}

	if (read_len) *read_len = data_len;
	return 0; 
}

double prng_rand(){
	return dist(mt);
}

int main(int argc, char const* argv[])
{
	if (argc < 5)
	{
		std::cout << "Error: Insufficient parameters!!\n";
		print_help_info();
		return -1;
	}

	int ret = 0;
	char output_filename[] = "0_1_dist_2m_prng_1.txt";

	/*1. Init qrng and prng */
	#ifdef __unix__
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "/dev/xdma0" });
	#else _WIN32
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "0" });
	#endif
	printf("\nqrng init ret: %d \n", qrng_get_status(qrng));

	prng_init();
	
	/* 2. get data from driver */
	std::vector<uint8_t> test_data(MB(8));
	int data_len = test_data.size();
	uint8_t* data_r = test_data.data();
	s32 bytes_read = 0;

	//3. open file to append to file
	const char* file_name = argv[1];
	int64_t num_length = atoll(argv[2]);// 2 * 1000 * 1000;//3 million
	int num_of_files = atoll(argv[3]);
	int data_type = PRNG_UDIST_DATA; 
        const char* data_type_str = argv[4]; 
	if (strcmp(data_type_str, "qrng_raw")==0 ) data_type = QRNG_RAW_DATA; 
	else if (strcmp(data_type_str, "qrng_udist") == 0) data_type = QRNG_UDIST_DATA;
	else if (strcmp(data_type_str, "prng_udist") == 0) data_type = PRNG_UDIST_DATA;
	else if (strcmp(data_type_str, "prng_raw") == 0) data_type = PRNG_RAW_DATA;
	
	else {
		std::cout << "Error: No valid data type was selected" << std::endl;
		print_help_info();
		return -3;
	}

	//if (num_length > 10000000000) num_length = 10000000000;
	if (num_of_files > 100) num_of_files = 100;
	
	std::cout << "To fetch: " << data_type_str << " data "  
		<< "and write to files with prefix: " << file_name
		<< ", size: " << num_length
		<< ", number of files: " << num_of_files 
		<< std::endl;

	//4. get random number and append to 
	for (int a = 0; a < num_of_files; a++) {

		char fn[200] = { 0 };
		snprintf(fn, sizeof(fn), "%s_%s_%d", file_name, data_type_str, a+1);
		std::ofstream ofs(fn, std::ofstream::out);
		std::cout << "\nFile: " << fn << ", " << (a + 1) << " of " << num_of_files << ", is_open? " << ofs.is_open()  << std::endl;

		auto begin = std::chrono::steady_clock::now();

		int64_t i = 0;
		while (i < num_length){
			//5. Fetch data and write to file 
			int64_t to_read = 0;

			if (data_type == QRNG_RAW_DATA || data_type == PRNG_RAW_DATA){
				bytes_read = 0;
				to_read =  num_length-i;
				to_read = (data_len < to_read)? data_len: to_read;
				ret = (data_type == QRNG_RAW_DATA)? qrng_get(qrng, data_r, to_read, &bytes_read): prng_get(data_r, to_read, &bytes_read);
				to_read = bytes_read; 
				i+=to_read;
			}else {
				double ret_double = (data_type==QRNG_UDIST_DATA)? qrng_urand(qrng):prng_rand(); 
				snprintf((char*)data_r, data_len, "%.12lf\n", ret_double);
				to_read= strlen((const char*)data_r);
				i++;
			}

			//checks if any error occurred in the function
			if (data_type == QRNG_RAW_DATA || data_type == QRNG_UDIST_DATA)
				if (qrng_get_status(qrng))printf("\t\tERROR! from qrng fn,  error/status code: %d\n",  qrng_get_status(qrng));

			auto end = std::chrono::steady_clock::now();
			double time_diff = 0.000001 * std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count();
			printf("\r Progress: %3.2f percent,  time: %.10fs", ((float)i / num_length) * 100, time_diff);

			ofs.write((const char*)data_r, to_read);
		}
		ofs.close();
	}
	
	printf("\n");
	return 0;
}


