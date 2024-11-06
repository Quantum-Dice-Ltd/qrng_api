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
#include <cstdint>
#include <iostream>
#include <vector>

#include "qrng_api.h" 
#define DEBUG_PRINT printf

/**
 * This function prints out the initial portion of an array of unsigned char
 * defined by the 'x' and 'y' arguments
 *
 * @param[in]    data    The unsigned char array
 * @param[in]    size    size of the array
 * @param[in]    x, y    length and height of the matrix
 */
template <typename T>
void print_data_stream(T* data, int size, int x, int y) {

	for (int i = 0; i < size; i++) {
		if ((i % x) == 0)
			DEBUG_PRINT("\n");
		if (i >= y * x)
			break;

		if constexpr (std::is_same<T, u8>::value)
			DEBUG_PRINT(" %02x", data[i]);
		else if constexpr (std::is_same<T, u16>::value)
			DEBUG_PRINT(" %04x", data[i]);
		else if constexpr (std::is_same<T, float>::value)
			DEBUG_PRINT(" %4.2f", data[i]);
		else
			DEBUG_PRINT(" -%x", data[i]);
	}
	DEBUG_PRINT("\n");
}

int main() {
	/** Initialize the QRNG api */
	#ifdef __unix__
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "/dev/xdma0" });
	#else //_WIN32
		QRNG* qrng = qrng_init_param({ QRNG_VERTEX_B1, "0" });
	#endif

	DEBUG_PRINT(" QRNG init ret: %d\n", qrng_get_status(qrng));

	std::vector<uint8_t> hashed_data(32);
	std::vector<uint16_t> ent_bits(hashed_data.size()/8);
	std::vector<float> cert_val(hashed_data.size()/8);

	std::vector<uint16_t> raw_ent(hashed_data.size() );

	qrng_get_with_ec(qrng, 
		hashed_data.data(), hashed_data.size(),
		ent_bits.data(), ent_bits.size(),
		cert_val.data(), cert_val.size());

	DEBUG_PRINT("\n QRNG get function returned: %d", qrng_get_status(qrng));
	if (qrng_get_status(qrng) != QRNG_SUCCESS) {
		DEBUG_PRINT("\n QRNG experienced ERROR [%d] while fetching data",
			qrng_get_status(qrng));
		goto exit;
	}
	
	DEBUG_PRINT("\n- QRNG hashed data: ");
	print_data_stream(hashed_data.data(), hashed_data.size(), 32, 1);
	DEBUG_PRINT("\n- QRNG certification data: ");
	print_data_stream(cert_val.data(), cert_val.size(), 32, 1);
	DEBUG_PRINT("\n- QRNG entropy bits: ");
	print_data_stream(ent_bits.data(), ent_bits.size(), 32, 1);
		
	qrng_get_raw_ent(qrng, raw_ent.data(), raw_ent.size(), NULL);
	DEBUG_PRINT("\n- QRNG raw entropy function returned: %d ", qrng_get_status(qrng));
	if (qrng_get_status(qrng) != QRNG_SUCCESS) {
		DEBUG_PRINT("\n QRNG experienced ERROR [%d] while fetching data",
			qrng_get_status(qrng));
		goto exit;
	}
	print_data_stream(raw_ent.data(), raw_ent.size(), 32, 1);

	exit: 
		qrng_deinit(qrng);
		return 0;
}

