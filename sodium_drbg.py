import ctypes
import ctypes.util
sodium = ctypes.cdll.LoadLibrary(ctypes.util.find_library('libsodium'))
keybytes = sodium.crypto_kdf_keybytes()
ctxbytes = sodium.crypto_kdf_contextbytes()
seedbytes = sodium.randombytes_seedbytes()
class KeyedCtrDRBG(object):
    def __init__(self):
        self.key = (ctypes.c_uint8 * keybytes)()
        self.ctx = (ctypes.c_char * ctxbytes)()
    def __repr__(self):
        keyl = ','.join(['0x{:02x}'.format(x) for x in self.key])
        ctxl = ','.join(['0x{:02x}'.format(int.from_bytes(x, 'little')) for x in self.ctx])
        return '{{[{}]:[{}]}}'.format(keyl, ctxl)
    def setRandomKey(self):
        key = (ctypes.c_uint8 * keybytes)()
        sodium.crypto_kdf_keygen(key)
        self.key = key
    def genSeedByInputScalar(self, s):
        seed = (ctypes.c_ubyte * seedbytes)()
        subkey_id = ctypes.c_uint64(s)
        sodium.crypto_kdf_derive_from_key(seed, len(seed), subkey_id, self.ctx, self.key)
        return seed
    def getUInt32(self, ctr):
        seed = self.genSeedByInputScalar(ctr)
        out_buff = (ctypes.c_ubyte * 4)()
        out = ctypes.c_uint32.from_buffer(out_buff)
        # MEMO: Data is written into out_buff as little endian.
        sodium.randombytes_buf_deterministic(out_buff, len(out_buff), seed)
        return out
    def getUInt64(self, ctr):
        seed = self.genSeedByInputScalar(ctr)
        out_buff = (ctypes.c_ubyte * 8)()
        out = ctypes.c_uint64.from_buffer(out_buff)
        # MEMO: Data is written into out_buff as little endian.
        sodium.randombytes_buf_deterministic(out_buff, len(out_buff), seed)
        return out
    def getBytes(self, ctr, bytesize):
        seed = self.genSeedByInputScalar(ctr)
        out_buff = (ctypes.c_ubyte * bytesize)()
        sodium.randombytes_buf_deterministic(out_buff, len(out_buff), seed)
        return out_buff
# vim: set expandtab foldmethod=marker :
