#pragma once

#include <array>
#include <memory>
#include <vector>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "tool.hpp"

namespace drbg {
class KeyedCtrDRBG_AES {
    /*
        NOTE: This implmentation is based on OpenSSL 1.0.2q,
        not latest version. Internal logic should be improved.
    */
public:
    using key_t = std::vector<unsigned char>;
    enum class KeySize {
        AES128,
        AES192,
        AES256,
    };
    static constexpr size_t blocksize = 128;
    static_assert(8 == CHAR_BIT);
    static_assert((blocksize % CHAR_BIT) == 0);
    static constexpr size_t blockbytes = blocksize / CHAR_BIT;
    using aes_block_t = std::array<unsigned char, blockbytes>;
    using buff_t = std::vector<unsigned char>;

private:
    const EVP_CIPHER *ci_;
    KeySize size_;
    key_t key_;

public:
    static void setRandomKey(KeyedCtrDRBG_AES &x) {
        assert(x.key_.size() > 0);
        {
            const auto status = RAND_bytes(x.key_.data(), x.key_.size());
            if (status != 1) {
                throw std::runtime_error(
                    ERR_error_string(ERR_get_error(), nullptr));
            }
        }
    }
    static const EVP_CIPHER *getCipher(const KeySize size) { // {{{
        switch (size) {
        case KeySize::AES128:
            return EVP_aes_128_ecb();
        case KeySize::AES192:
            return EVP_aes_192_ecb();
        case KeySize::AES256:
            return EVP_aes_256_ecb();
        default:
            throw std::invalid_argument("KeyedCtrDRBG_AES:getCipher: unknown");
        }
    } // }}}
    static const std::string &getName(const KeySize size) { // {{{
        static const std::string &aes128 = "AES128";
        static const std::string &aes192 = "AES192";
        static const std::string &aes256 = "AES256";
        switch (size) {
        case KeySize::AES128:
            return aes128;
        case KeySize::AES192:
            return aes192;
        case KeySize::AES256:
            return aes256;
        default:
            throw std::invalid_argument("KeyedCtrDRBG_AES:getName: unknown");
        }
    } // }}}
    explicit KeyedCtrDRBG_AES(const KeySize size, const key_t &key)
        : ci_(getCipher(size)), size_(size), key_(key) { // {{{
        assert(ci_ != nullptr);
        const auto keysize = EVP_CIPHER_key_length(ci_);
        assert(size_t(keysize) == key_.size());
    } // }}}
    explicit KeyedCtrDRBG_AES(const KeySize size)
        : ci_(getCipher(size)), size_(size), key_({}) { // {{{
        assert(ci_ != nullptr);
        const auto keysize = EVP_CIPHER_key_length(ci_);
        key_.resize(keysize);
    } // }}}
    explicit KeyedCtrDRBG_AES() : KeyedCtrDRBG_AES(KeySize::AES128) {}
    friend std::ostream &operator<<(std::ostream &o,
                                    const KeyedCtrDRBG_AES &x) { // {{{
        o << "[" << getName(x.size_) << ":" << tool::to_hex(x.key_) << "]";
        return o;
    } // }}}
    void setKey(const key_t &key) { // {{{
        assert(key.size() == key_.size());
        using std::begin;
        using std::end;
        std::copy(begin(key), end(key), begin(key_));
    } // }}}
    const key_t &getKey() const { return key_; }

private:
    struct Ctx { // {{{
        EVP_CIPHER_CTX *ctx_;
        Ctx() // {{{
            : ctx_(nullptr) {
            ctx_ = EVP_CIPHER_CTX_new();
            if (ctx_ == nullptr) {
                throw std::runtime_error(
                    "KeyedCtrDRBG_AES:Ctx:ctor:OpenSSL:EVP_CIPHER_CTX_new: failed");
            }
        } // }}}
        ~Ctx() { // {{{
            if (ctx_ != nullptr) {
                EVP_CIPHER_CTX_free(ctx_);
            }
        } // }}}
    }; // }}}
public:
    uint32_t getUInt32(const uint32_t ctr) const { // {{{
        aes_block_t out{0};
        static_assert(blockbytes > sizeof(uint32_t));
        // out[sizeof(uint32_t)] = 0xff;
        getBytes(out, ctr);
        uint32_t ret;
        tool::copy_as_uint(ret, out);
        return ret;
    } // }}}
    uint64_t getUInt64(const uint64_t ctr) const { // {{{
        aes_block_t out{0};
        static_assert(blockbytes > sizeof(uint64_t));
        getBytes(out, ctr);
        uint64_t ret;
        tool::copy_as_uint(ret, out);
        return ret;
    } // }}}
    void getBytes(aes_block_t &out, const uint64_t ctr) const { // {{{
        Ctx ctx;
        {
            const auto status = EVP_EncryptInit_ex(ctx.ctx_, ci_, nullptr,
                                                   key_.data(), nullptr);
            if (status != 1) {
                throw std::runtime_error(
                    "KeyedCtrDRBG_AES:getUInt32: init failed");
            }
        }
        EVP_CIPHER_CTX_set_padding(ctx.ctx_, 0);
        aes_block_t ctrbytes{0};
        tool::copy_as_bytes_little(ctrbytes, ctr);
        {
            int outlen;
            const auto status =
                EVP_EncryptUpdate(ctx.ctx_, out.data(), &outlen,
                                  ctrbytes.data(), ctrbytes.size());
            if (status != 1) {
                throw std::runtime_error(
                    "KeyedCtrDRBG_AES:getUInt32: update failed");
            }
            assert(outlen == blockbytes);
        }
        {
            int outlen;
            const auto status =
                EVP_EncryptFinal_ex(ctx.ctx_, out.data() + out.size(), &outlen);
            if (status != 1) {
                throw std::runtime_error(
                    "KeyedCtrDRBG_AES:getUInt32: final failed");
            }
            assert(outlen == 0);
        }
    } // }}}
};
} // namespace drbg
// vim: set expandtab foldmethod=marker:
