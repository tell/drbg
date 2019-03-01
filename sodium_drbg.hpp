#pragma once

#include <cassert>
#include <cerrno>
#include <cstring>
#include <array>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <sodium.h>

#include "tool.hpp"

namespace drbg {
class KeyedCtrDRBG_Sodium { // {{{
public:
    constexpr static auto keybytes = crypto_kdf_KEYBYTES;
    constexpr static auto ctxbytes = crypto_kdf_CONTEXTBYTES;
    constexpr static auto seedbytes = randombytes_SEEDBYTES;
    static_assert(randombytes_SEEDBYTES >= crypto_kdf_BYTES_MIN);
    static_assert(randombytes_SEEDBYTES <= crypto_kdf_BYTES_MAX);
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
    static void setRandomKey(KeyedCtrDRBG_Sodium &x) { x.key_ = randomKey(); }
    explicit KeyedCtrDRBG_Sodium(const key_t &key, const ctx_t &ctx)
        : key_(key), ctx_(ctx) {}
    explicit KeyedCtrDRBG_Sodium(const key_t &key)
        : KeyedCtrDRBG_Sodium(key, {0}) {}
    explicit KeyedCtrDRBG_Sodium() : KeyedCtrDRBG_Sodium({0}) {}
    friend std::ostream &operator<<(std::ostream &o,
                                    const KeyedCtrDRBG_Sodium &x) { // {{{
        constexpr size_t len = keybytes;
        assert(len > 0);
        std::stringstream s;
        s << "[" << tool::to_hex(x.key_) << ":" << tool::to_hex(x.ctx_) << "]";
        o << s.str();
        return o;
    } // }}}
    void setKey(const key_t &key) { key_ = key; }
    const key_t &getKey() const { return key_; }
    void setCtx(const ctx_t &ctx) { ctx_ = ctx; }
    const ctx_t &getCtx() const { return ctx_; }

public:
    uint32_t getUInt32(const uint32_t ctr) const { // {{{
        std::array<buff_t::value_type, sizeof(uint32_t)> out;
        getBytes(out, ctr);
        uint32_t ret;
        tool::copy_as_uint(ret, out);
        return ret;
    } // }}}
    uint64_t getUInt64(const uint64_t ctr) const { // {{{
        std::array<buff_t::value_type, sizeof(uint64_t)> out;
        getBytes(out, ctr);
        uint64_t ret;
        tool::copy_as_uint(ret, out);
        return ret;
    } // }}}
    template <class T, class U>
    void getBytes(T &out, const U ctr) const { // {{{
        seed_t seed;
        const auto status = crypto_kdf_derive_from_key(
            seed.data(), seed.size(), ctr, ctx_.data(), key_.data());
        if (status != 0) {
            std::string errmsg = "KeyedCtrDRBG_Sodium:getBytes:KDF";
            if (errno != 0) {
                errmsg += ": ";
                errmsg += std::strerror(errno);
            }
            throw std::invalid_argument(errmsg);
        }
        randombytes_buf_deterministic(out.data(), out.size(), seed.data());
    } // }}}
}; // }}}
} // namespace drbg
// vim: set expandtab foldmethod=marker:
