import qrnglib
import pytest

def test_init_fn():
    """
    Tests the constructor of the qrng library with inputs 
    that are expected to raise an exception within it
    """

    with pytest.raises(TypeError, match=r".*product name.*") as e_info:
        qdQrng = qrnglib.QdQrng(1,"mydev")

    with pytest.raises(TypeError, match=r".*device name.*") as e_info:
        qdQrng = qrnglib.QdQrng("myprod",1)

    with pytest.raises(TypeError) as e_info:
        qdQrng = qrnglib.QdQrng(1,1)
    
    with pytest.raises(ValueError, match=r".*does not exist.*") as e_info:
        qdQrng = qrnglib.QdQrng("non-existent-product","mydev")

def test_get_fn():
    """
    Tests the qrng get functions. 
    No errors are expected here
    """
    qdQrng = qrnglib.QdQrng("VERTEX_A1","/dev/xdma0")
    #qdQrng = qrnglib.QdQrng("VERTEX_A1","0") 
    assert qdQrng.qrng_cptr != 0
    assert qdQrng.get_status() == 0
    print(f" {'QRNG rand (integer 0 - 32767)' :30}  {qdQrng.rand()}" )
    print(f" {'QRNG urand (double 0 - 1)' :30}  {qdQrng.urand()}" )
    print(f" {'QRNG get ' :30}  {qdQrng.get(15)}" )
    print(f" {'QRNG get raw ent ' :30}  {qdQrng.get_raw_ent(1)}" )
    print(f" {'QRNG get with ec' :30}  {qdQrng.get_with_ec(32)}" )



