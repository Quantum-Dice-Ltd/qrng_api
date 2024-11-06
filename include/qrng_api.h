/** 
* @file 	qrng_api.h
* @brief 	QD QRNG API
*
* @note		All get functions do not zerorise buffer.
* 			This means that in cases where the function
* 			is unable to read up to the "size" of the
* 			passed in buffer "data" the buffer would first be
* 			filled with the available data, but the rest
* 			of the buffer would retain it's previous value.
*
* @author	Clifford Olawaiye
* @date		15/05/2024
*/

#pragma once 

#ifdef _WIN32 
#	include <windows.h>
#else
#	include<unistd.h>
#endif

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed long s32;
typedef unsigned long u32;
typedef signed long long s64;
typedef unsigned long long u64;


typedef enum {
	QRNG_ERROR_WRONG_DATA_FORMAT = -16,
	QRNG_ERROR_INTERNAL_CH_ERROR = -15,
	QRNG_ERROR_NET_RETRIES_EXCEEDED = -14,
	QRNG_ERROR_NET_TIMEOUT = -13,
	QRNG_ERROR_MULTIPLE_OF_8_REQUIRED = -11,

	QRNG_ERROR_INSUFFICIENT_ENTHROPY = -10,
	QRNG_ERROR_INTERNAL_MEMORY = -9,
	QRNG_ERROR_READING_DEVICE = -7,
	QRNG_ERROR_INCOMPLETE_DATA = -6,
	QRNG_ERROR_OPENING_DEVICE = -5,

	QRNG_ERROR_NULL_PTR = -2,
	QRNG_NO_DEVICE_FOUND = -1,
	QRNG_SUCCESS = 0,
}Qrng_status;

typedef enum {
	QRNG_VERTEX_A1, 
	QRNG_VERTEX_B1,
	QRNG_APEXTREME,
}Qrng_board_type;

typedef struct {
	Qrng_board_type board_type;
	const char* dev_name;
}Qrng_init_param;

typedef void QRNG;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* Initialize the qrng hardware with the default configurations
	* 
	* @return	QRNG*	pointer to the QRNG object
	*/
	QRNG *qrng_init();

	/**
	* Initialize the qrng hardware with the parameters passed in
	*
	* @param[in] 	Qrng_init_param		A struct that holds the
	* 									parameters to initialize
	* 									the qrng with.
	*
	* @return	QRNG*	pointer to the QRNG object
	*/
	QRNG *qrng_init_param(Qrng_init_param init_param);

	/**
	* De-initialize the qrng hardware and release the allocated resources
	*
	* @param[in]	QRNG*	The qrng object to de-initialize
	*/
	void qrng_deinit(QRNG* qrng);

	/**
	* This function is used to get an array of random numbers directly
	* from the hardware. It is important that the qrng_init() function
	* is called once before using this function in the main program,
	* i.e. after calling the qrng_init() once you can call
	* the qrng_get() function as many times as needed.
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object to read from
	* @param[out]	data	Buffer to receive the random numbers
	* @param[in]	size	Size of the buffer 
	* @param[out]	bytes_read 	Returns the size of data read from the qrng
	*
	* @return	QRNG_status
	*
	*/
	int qrng_get(QRNG* qrng,
				u8* data,
				s32 size,
				s32* bytes_read);

	/**
	* Get one random number of type 'int' from the qrng
	* 
	* This function is to be used like the cpp rand() function,
	* with this there would be no need to call the srand() function first.
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object to read from
	*
	* @return	Random number of type 'int'
	*/
	int qrng_rand(QRNG* qrng);

	/**
	* Get one random number (uniform distribution between 0 and 1)
	* of type 'double'  from the qrng
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object to read from
	*
	* @return	Random number of type '64 bits float' between 0 and 1
	*/
	double qrng_urand(QRNG* qrng);

	/**
	* Get one random number (uniform distribution between 0 and 1)
	* of type 'double'  from the qrng
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object to read from
	*
	* @return	Random number of type '32 bits float' between 0 and 1
	*/
	float qrng_urand2(QRNG* qrng);

	/**
	* Get the status of the last API function call
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object
	*
	* @return QRNG_status
	*/
	int qrng_get_status(QRNG* qrng);

	/**
	* This function is used to get an array of raw entropy bits
	*
	* @param[in] 	QRNG* 	Pointer to the QRNG object
	* @param[out]	data	16 bits buffer to receive the raw entropy data
	* @param[in]	size	Size of the buffer
	* @param[out]	bytes_read	Returns the size of the data read
	* 							from the qrng
	*
	* @return	QRNG_status
	*
	*/
	int qrng_get_raw_ent(QRNG* qrng,
						u16* data,
						s32 data_size,
						s32* bytes_read);


	/**
	 * Get QRNG Random data with Entropy bits and Certification value array
	 *
	 * Returns a stream of random numbers as well as the entropy bits
	 * and certification value for each group of 64bits(8 bytes).
	 * This also implies that users can only read hashed data in multiples
	 * of 8 bytes, if the "size" passed in is not equal to 8 the function
	 * would return a QRNG_ERROR_MULTIPLE_OF_8_REQUIRED error.
	 *
	 * @param[in] 	QRNG* 	Pointer to the QRNG object
	 * @param[out] 	data		Pointer to 8bits array allocation to
								receive the hashed random numbers
	 * @param[in] 	data_size	Size of allocation "data",
	 * 							must be a multiple of 8
	 * @param[out] 	ent_bits	Pointer to a 16bits array allocation
	 * 							to receive the Entropy bits
	 * @param[in] 	eb_size		Size of ent_bits array
	 * @param[out] 	cert_val	Pointer to a floats array allocation
	 * 							to receive the certification values
	 * @param[in] 	cv_size		Size of cert_val array
	 *
	 * @return 		QRNG_status
	 */
	int qrng_get_with_ec(QRNG* qrng,
						u8* data,
						s32 size,
						u16* ent_bits,
						s32 eb_size,
						float* cert_val,
						s32 cv_size) ;

#ifdef __cplusplus
}
#endif

