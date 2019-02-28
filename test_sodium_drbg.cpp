#include <iostream>

#include <cybozu/test.hpp>

#include "sodium_drbg.hpp"

using namespace std;
using namespace drbg;
using namespace tool;

CYBOZU_TEST_AUTO(print_info_sodium) { // {{{
    cout << "libsodium: version = " << sodium_version_string() << endl;
    const auto major = sodium_library_version_major();
    const auto minor = sodium_library_version_minor();
    cout << "libsodium: major version = " << major << endl;
    cout << "libsodium: minor version = " << minor << endl;
    cout << "libsodium: crypto_kdf_KEYBYTES = " << KeyedCtrDRBG_Sodium::keybytes
         << endl;
    cout << "libsodium: crypto_kdf_BYTES_MIN = " << crypto_kdf_BYTES_MIN
         << endl;
    cout << "libsodium: crypto_kdf_CONTEXTBYTES = "
         << KeyedCtrDRBG_Sodium::ctxbytes << endl;
    cout << "libsodium: randombytes_SEEDBYTES = "
         << KeyedCtrDRBG_Sodium::seedbytes << endl;
} // }}}
CYBOZU_TEST_AUTO(test_KeyedCtrDRBG_Sodium) { // {{{
    for (size_t j = 0; j < 10; j++) {
        KeyedCtrDRBG_Sodium a;
        KeyedCtrDRBG_Sodium::setRandomKey(a);
        for (size_t i = 0; i < 10; i++) {
            {
                const auto x = a.getUInt32(i);
                const auto y = a.getUInt32(i);
                CYBOZU_TEST_EQUAL(x, y);
            }
            {
                const auto x = a.getUInt64(i);
                const auto y = a.getUInt64(i);
                CYBOZU_TEST_EQUAL(x, y);
            }
        }
    }
} // }}}
CYBOZU_TEST_AUTO(test_KeyedCtrDRBG_Sodium_with_fixed_key) { // {{{
    const KeyedCtrDRBG_Sodium::key_t key{
        0xb9, 0xf2, 0xaa, 0x37, 0x0e, 0xe2, 0xdf, 0x73, 0x47, 0xe9, 0xa5,
        0x61, 0x5a, 0x00, 0x31, 0x9b, 0x0c, 0xde, 0x9b, 0xf0, 0xd5, 0x94,
        0x7a, 0x66, 0x99, 0x25, 0x8e, 0xfd, 0x5e, 0x02, 0x34, 0xb4};
    const size_t nsamples = 1000;
    vector<uint32_t> samples32(nsamples);
    vector<uint64_t> samples64(nsamples);
    {
        KeyedCtrDRBG_Sodium a;
        a.setKey(key);
        CYBOZU_TEST_EQUAL_ARRAY(a.getKey(), key, KeyedCtrDRBG_Sodium::keybytes);
        for (size_t i = 0; i < nsamples; i++) {
            samples32[i] = a.getUInt32(i);
            samples64[i] = a.getUInt64(i);
            CYBOZU_TEST_EQUAL(samples32[i], uint32_t(samples64[i]));
        }
    }
    for (size_t j = 0; j < 100; j++) {
        KeyedCtrDRBG_Sodium a;
        a.setKey(key);
        CYBOZU_TEST_EQUAL_ARRAY(a.getKey(), key, KeyedCtrDRBG_Sodium::keybytes);
        for (size_t i = 0; i < nsamples; i++) {
            {
                const auto x = a.getUInt32(i);
                CYBOZU_TEST_EQUAL(x, samples32[i]);
            }
            {
                const auto x = a.getUInt64(i);
                CYBOZU_TEST_EQUAL(x, samples64[i]);
            }
        }
    }
} // }}}
CYBOZU_TEST_AUTO(test_KeyedCtrDRBG_Sodium_one_shot) { // {{{
    KeyedCtrDRBG_Sodium a;
    KeyedCtrDRBG_Sodium::setRandomKey(a);
    {
        const auto x = a.getUInt32(1);
        const auto y = a.getUInt32(1);
        CYBOZU_TEST_EQUAL(x, y);
    }
    {
        const auto x = a.getUInt64(3);
        const auto y = a.getUInt64(3);
        CYBOZU_TEST_EQUAL(x, y);
    }
} // }}}
// vim: set expandtab foldmethod=marker:
