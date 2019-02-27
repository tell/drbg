#include <cybozu/test.hpp>

#include "openssl_aes_drbg.hpp"

using namespace std;
using namespace drbg;

CYBOZU_TEST_AUTO(test_simple) {
    const size_t nloop = 10000;
    for (size_t i = 0; i < nloop; i++) {
        KeyedCtrDRBG_AES a;
        KeyedCtrDRBG_AES::setRandomKey(a);
        cout << a << endl;
        cout << a.getUInt32(0) << endl;
        cout << a.getUInt32(1) << endl;
        cout << a.getUInt32(1250) << endl;
    }
}

// vim: set expandtab foldmethod=marker:
