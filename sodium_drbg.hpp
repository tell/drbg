#pragma once

#include <cassert>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <sodium.h>

#define my_static_assert(x) static_assert((x), #x);

namespace tool {

template <class T>
inline std::string to_hex(const T &x, const std::string &delim = ",") { // {{{
    size_t len = x.size();
    std::stringstream s;
    s << "{";
    if (len > 0) {
        s << std::hex;
        s << "0x" << std::setw(2 * sizeof(typename T::value_type))
          << std::setfill('0') << static_cast<uint_fast64_t>(x[0]);
        for (size_t i = 1; i < len; i++) {
            s << delim << "0x" << std::setw(2 * sizeof(typename T::value_type))
              << std::setfill('0') << static_cast<uint_fast64_t>(x[i]);
        }
    }
    s << "}";
    return s.str();
} // }}}
class keyed_ctr_drbg { // {{{
public:
    constexpr static auto keybytes = crypto_kdf_KEYBYTES;
    constexpr static auto ctxbytes = crypto_kdf_CONTEXTBYTES;
    constexpr static auto seedbytes = randombytes_SEEDBYTES;
    using key_t = std::array<uint8_t, keybytes>;
    using ctx_t = std::array<char, ctxbytes>;
    using seed_t = std::array<unsigned char, seedbytes>;
    using buff_t = std::vector<unsigned char>;

private:
    key_t key_;
    ctx_t ctx_;

public:
    static key_t randomKey() { // {{{
        key_t key;
        crypto_kdf_keygen(key.data());
        return key;
    } // }}}
    static void setRandomKey(keyed_ctr_drbg &x) { x.key_ = randomKey(); }
    keyed_ctr_drbg(const key_t &key, const ctx_t &ctx = {0})
        : key_(key), ctx_(ctx) {}
    keyed_ctr_drbg() : keyed_ctr_drbg({0}) {}
    friend std::ostream &operator<<(std::ostream &o,
                                    const keyed_ctr_drbg &x) { // {{{
        constexpr size_t len = keybytes;
        assert(len > 0);
        std::stringstream s;
        s << "[" << to_hex(x.key_) << ":" << to_hex(x.ctx_) << "]";
        o << s.str();
        return o;
    } // }}}
    void setKey(const key_t &key) { key_ = key; }
    const key_t &getKey() const { return key_; }
    void setCtx(const ctx_t &ctx) { ctx_ = ctx; }
    const ctx_t &getCtx() const { return ctx_; }

public:
    uint32_t getUInt32(const uint32_t ctr = 0) { // {{{
        seed_t seed;
        crypto_kdf_derive_from_key(seed.data(), seed.size(), ctr, ctx_.data(),
                                   key_.data());
        buff_t out(sizeof(uint32_t));
        randombytes_buf_deterministic(out.data(), sizeof(uint32_t),
                                      seed.data());
        uint32_t ret = static_cast<uint32_t>(out[0]) +
                       (static_cast<uint32_t>(out[1]) << 8) +
                       (static_cast<uint32_t>(out[2]) << (8 * 2)) +
                       (static_cast<uint32_t>(out[3]) << (8 * 3));
        return ret;
    } // }}}
    uint64_t getUInt64(const uint64_t ctr = 0) { // {{{
        seed_t seed;
        crypto_kdf_derive_from_key(seed.data(), seed.size(), ctr, ctx_.data(),
                                   key_.data());
        buff_t out(sizeof(uint64_t));
        randombytes_buf_deterministic(out.data(), sizeof(uint64_t),
                                      seed.data());
        uint64_t ret = static_cast<uint64_t>(out[0]) +
                       (static_cast<uint64_t>(out[1]) << 8) +
                       (static_cast<uint64_t>(out[2]) << (8 * 2)) +
                       (static_cast<uint64_t>(out[3]) << (8 * 3)) +
                       (static_cast<uint64_t>(out[4]) << (8 * 4)) +
                       (static_cast<uint64_t>(out[5]) << (8 * 5)) +
                       (static_cast<uint64_t>(out[6]) << (8 * 6)) +
                       (static_cast<uint64_t>(out[7]) << (8 * 7));
        return ret;
    } // }}}
    void getBytes(buff_t &out, const uint64_t ctr = 0) { // {{{
        seed_t seed;
        crypto_kdf_derive_from_key(seed.data(), seed.size(), ctr, ctx_.data(),
                                   key_.data());
        randombytes_buf_deterministic(out.data(), out.size(), seed.data());
    } // }}}
}; // }}}
} // namespace tool

// vim: set expandtab foldmethod=marker:
