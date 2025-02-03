import platform
import ctypes
import os

# load the shared library or DLL 
THIS_FILE_DIRECTORY = os.path.join(os.path.dirname(os.path.realpath(__file__)), "../../lib")
if platform.system() == 'Linux': 
	qrnglibso =  os.path.join(THIS_FILE_DIRECTORY, 'linux/libqrng_vertex.so') 
	qrnglibhdlr = ctypes.CDLL(qrnglibso)
else: 
	qrnglibdll  = os.path.join(THIS_FILE_DIRECTORY, 'qrng_vertex.dll')
	qrnglibhdlr = ctypes.WinDLL(qrnglibdll)

# Define the argument and return types for functions 
# that have a different return type

qrnglibhdlr.qrng_bridge_product_name_to_enum.argtypes = [ctypes.c_char_p]
qrnglibhdlr.qrng_bridge_product_name_to_enum.restype = ctypes.c_int

qrnglibhdlr.qrng_bridge_init_param.argtypes = [ctypes.c_int, ctypes.c_char_p]
qrnglibhdlr.qrng_bridge_init_param.restype = ctypes.c_void_p

qrnglibhdlr.qrng_get_status.argtypes = [ctypes.c_void_p]
qrnglibhdlr.qrng_get_status.restype = ctypes.c_int

qrnglibhdlr.qrng_rand.argtypes = [ctypes.c_void_p]
qrnglibhdlr.qrng_rand.restype = ctypes.c_int

qrnglibhdlr.qrng_urand.argtypes = [ctypes.c_void_p]
qrnglibhdlr.qrng_urand.restype = ctypes.c_double

qrnglibhdlr.qrng_get.argtypes = [ctypes.c_void_p, 
                                 ctypes.POINTER(ctypes.c_uint8),
                                 ctypes.c_int32, 
                                 ctypes.c_void_p]
qrnglibhdlr.qrng_get.restype = ctypes.c_int

qrnglibhdlr.qrng_get_raw_ent.argtypes = [ctypes.c_void_p,
                                         ctypes.POINTER(ctypes.c_uint16),
                                         ctypes.c_int32,
                                         ctypes.c_void_p]
qrnglibhdlr.qrng_get_raw_ent.restype = ctypes.c_int

qrnglibhdlr.qrng_get_with_ec.argtypes = [ctypes.c_void_p,
                                         ctypes.POINTER(ctypes.c_uint8),
                                         ctypes.c_int32, 
                                         ctypes.POINTER(ctypes.c_uint16),
                                         ctypes.c_int32, 
                                         ctypes.POINTER(ctypes.c_float),
                                         ctypes.c_int32] 
qrnglibhdlr.qrng_get_with_ec.restype = ctypes.c_int

qrnglibhdlr.qrng_deinit.argtypes = [ctypes.c_void_p]


class QdQrng:
    qrng_cptr = None
    def __init__(self, product_name: str, dev_name: str) :
        """
        Initialize the qrng device by specifying the 
        product name and device name.

        Parameters: 
            Product_name (str): e.g. VERTEX_A1, VERTEX_B1, APEXTREME
            dev_name (str):     e.g. /dev/xdma0
        """
        #verify the product name and device name are string
        if not isinstance(product_name, str): 
              raise TypeError("Only string is allowed for product name")
        if not isinstance(dev_name, str): 
              raise TypeError("Only string is allowed for device name")
        cstr_prod_name = product_name.encode('utf-8')
        cstr_dev_name = dev_name.encode('utf-8')

        product_enum_num = qrnglibhdlr.qrng_bridge_product_name_to_enum(
                                            cstr_prod_name)
        
        if product_enum_num < 0: 
            raise ValueError(f"Product '{product_name}' does not exist")
        
        self.qrng_cptr = qrnglibhdlr.qrng_bridge_init_param(
                                            product_enum_num, 
                                            cstr_dev_name)

        
    
    def __del__(self):
        if self.qrng_cptr== None: 
            return
        qrnglibhdlr.qrng_deinit(self.qrng_cptr)
        self.qrng_cptr = None

    def get_status(self): 
        """
        Returns the status info about the last operation 
        performed by the API

        Returns: 
            num (int): status code
        """
        return qrnglibhdlr.qrng_get_status(self.qrng_cptr)
    
    def rand(self): 
        """
        Returns a random number between 0 and 32767

        Returns: 
            num (int): A random number between 0 and 32767 
        """
        return qrnglibhdlr.qrng_rand(self.qrng_cptr)

    def urand(self): 
        """
        Returns a random number between 0 and 1

        Returns: 
            num (float): A random number between 0 and 1 
        """
        return qrnglibhdlr.qrng_urand(self.qrng_cptr)

    def get(self, cnt): 
        """
        Returns 'cnt' dynamicaly hashed random numbers 

        Parameters: 
            cnt (int): The amount of random numbers to return 
        
        Returns: 
            data_buf (list): A list of 8bits random numbers 
                            i.e. each number is between 
                            0 and 255 
        """
        ret_len = ctypes.c_uint32(0)
        
        data_buf = (ctypes.c_ubyte * cnt)()
        # ptr_cast = ctypes.cast(data_buf, ctypes.c_char_p) 
        ret = qrnglibhdlr.qrng_get( self.qrng_cptr, 
                                    data_buf, 
                                    cnt, 
                                    ctypes.byref(ret_len) ) 
            
        if ret < 0 :
             raise Exception(f"Error in cCode: {ret}")
        
        return data_buf[:]
    

    def get_raw_ent(self, cnt: int) -> list: 
        """
        Returns 'cnt' raw entropy 
            
        Parameters: 
            cnt (int): The amount of raw entropy numbers to return 
        
        Returns: 
            data_buf (list): A list of 16bits raw entropy 
                            i.e. each number is between 
                            0 and 65535 
        """
        ret_len = ctypes.c_uint32(0)
        data_buf = (ctypes.c_uint16 * cnt)()
        ret = qrnglibhdlr.qrng_get_raw_ent( self.qrng_cptr, 
                                           data_buf, 
                                           cnt, 
                                           ctypes.byref(ret_len) ) 
            
        if ret < 0 :
            raise Exception(f"Error in cCode: {ret}")        
        return data_buf[:]

    def get_with_ec(self, cnt: int) -> list: 
        """
        Returns 'cnt' hashed random numbers with their certification
        information. 
            
        Parameters: 
            cnt (int): The amount of random numbers to return
                        NB: this must be a muliple of 8 
        
        Returns: 
            data_buf (list): A list of 8bits random numbers 
                            i.e. each number is between 
                            0 and 255 
            ent_bits (list): A list of 16bits minimum raw entropy
            cert_val (list): A list of floats certification value 
        """
        if cnt < 1 or cnt % 8: 
             raise ValueError(
                  f"Function expects multiple of 8, cnt: {cnt} ")
        
        cnt2 = int(cnt/8)
        data_buf = (ctypes.c_uint8 * cnt)()
        ent_bits = (ctypes.c_uint16 * cnt2)()
        cert_val = (ctypes.c_float * cnt2)()
        ret = qrnglibhdlr.qrng_get_with_ec( self.qrng_cptr, 
                                           data_buf, cnt,
                                           ent_bits, cnt2, 
                                           cert_val, cnt2 ) 
            
        if ret < 0 :
            raise Exception(f"Error in cCode: {ret}")        
        return data_buf[:], ent_bits[:], cert_val[:]

    def shuffle(self, x):	
        int_class = int
        for i in reversed(range(1, len(x))):
            # swap current element with a different member in the list
            j = int_class(self.urand(self.qrng_cptr) * (i+1))
            x[i], x[j] = x[j], x[i]
        

