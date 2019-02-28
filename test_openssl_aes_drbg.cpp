#include <cybozu/test.hpp>

#include "openssl_aes_drbg.hpp"

using namespace std;
using namespace drbg;

CYBOZU_TEST_AUTO(print_info_openssl) {
    cout << "OpenSSL: version = " << OPENSSL_VERSION_TEXT << endl;
    cout << "OpenSSL: version number = 0x" << hex << OPENSSL_VERSION_NUMBER << endl;
}
CYBOZU_TEST_AUTO(test_simple) { // {{{
    const size_t nloop = 10000;
    const KeyedCtrDRBG_AES::KeySize tbl[] = {
        KeyedCtrDRBG_AES::KeySize::AES128,
        KeyedCtrDRBG_AES::KeySize::AES192,
        KeyedCtrDRBG_AES::KeySize::AES256,
    };
    for (auto &&keysize : tbl) {
        KeyedCtrDRBG_AES x(keysize);
        KeyedCtrDRBG_AES::setRandomKey(x);
        const auto a = x.getUInt32(1250);
        const auto b = x.getUInt64(1250);
        const auto key{x.getKey()};
        KeyedCtrDRBG_AES y(keysize, key);
        const auto c = x.getUInt32(1250);
        const auto d = x.getUInt64(1250);
        CYBOZU_TEST_EQUAL(a, c);
        CYBOZU_TEST_EQUAL(b, d);
        cout << "a = 0x" << hex << a << endl;
        cout << "b = 0x" << hex << uint32_t(b) << endl;
        for (size_t i = 0; i < nloop; i++) {
            const auto t = x.getUInt32(i);
            tool::unused(t);
        }
    }
} // }}}

// vim: set expandtab foldmethod=marker:
