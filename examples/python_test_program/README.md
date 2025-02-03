--- 

Title: "Quantum Dice QRNG python API"

Brief description: "This directory contains the python API wrapper for Quantum Dice QRNG "

Copyright: "(c) Quantum Dice. All rights reserved"

---


## Overview 
The following are included in this directory: 
1. qrnglib.dll: Windows dynamic library 
2. libqrnglib.so: Linux shared library 
3. qrnglib.py: Python wrapper script
4. test.py: Python test script

## Requirements
1. Quantum Dice QRNG hardware
2. OS: Windows 10 or Ubuntu 22.04 
3. Basic knowledge of Python programming 

## How to run the program: 

1. Install pytest (if it has not been installed yet)
    `pip install -U pytest`
2. Run pytest 
    `python3 -m pytest -rP `
    or 
    `pytest -vs `

Output:
```
======================================================= PASSES ========================================================
_____________________________________________________ test_get_fn _____________________________________________________
------------------------------------------------ Captured stdout call -------------------------------------------------
 QRNG rand (integer 0 - 32767)   5442
 QRNG urand (double 0 - 1)       0.8328915152847891
 QRNG get                        [156, 110, 106, 174, 8, 81, 112, 101, 30, 223, 241, 104, 128, 94, 222]
 QRNG get raw ent                [2251]
 QRNG get with ec                ([64, 233, 20, 223, 3, 7, 44, 109], [120], [67.15293884277344])
================================================== 1 passed in 0.13s ==================================================
```

From the output above, 
- qrng_rand returns a random number between 0 and 32767
- qrng_urand returns a uniform distribution between 0 and 1
- qrng_get returns a list of 8 bits random numbers 
- qrng_get_raw_ent returns a list of 16 bits raw entropy values 
- qrng_get_with_ec returns a list of hashed data along with it's certification and entropy bits. 



