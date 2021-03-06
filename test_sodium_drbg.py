import ctypes
import sodium_drbg
def test_drbg():
    key_t = (ctypes.c_uint8 * sodium_drbg.keybytes)
    sample_key = key_t(*[0xb9,0xf2,0xaa,0x37,0x0e,0xe2,0xdf,0x73,0x47,0xe9,0xa5,0x61,0x5a,0x00,0x31,0x9b,0x0c,0xde,0x9b,0xf0,0xd5,0x94,0x7a,0x66,0x99,0x25,0x8e,0xfd,0x5e,0x02,0x34,0xb4])
    sample_ui64_ctxctr0 = 3473560095213287730
    x = sodium_drbg.KeyedCtrDRBG()
    x.key = sample_key
    o = x.getUInt64(0)
    assert o.value == sample_ui64_ctxctr0
# vim: set expandtab foldmethod=marker :
