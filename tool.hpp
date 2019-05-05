#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#define my_static_assert(x) static_assert((x), #x);
#define PUT(x)                                                                 \
    do {                                                                       \
        std::cout << #x << "=" << (x) << std::endl;                            \
    } while (false)

namespace drbg {
namespace tool {
using swallow = std::initializer_list<int>;
template <class... Args> void unused(Args &&... args) {
    (void) swallow{(void(args), 0)...};
}
template <class T>
inline std::string to_hex(const T &x, const std::string &delim = ",") { // {{{
    size_t len = x.size();
    std::stringstream s;
    s << "{";
    if (len > 0) {
        s << std::hex;
        s << "0x" << std::setw(2 * sizeof(typename T::value_type))
          << std::setfill('0') << uint_fast64_t(x[0]);
        for (size_t i = 1; i < len; i++) {
            s << delim << "0x" << std::setw(2 * sizeof(typename T::value_type))
              << std::setfill('0') << uint_fast64_t(x[i]);
        }
    }
    s << "}";
    return s.str();
} // }}}
namespace impl {
#if 0
template <class T, class Int, std::size_t... I>
constexpr void
packed_copy_as_bytes_little(T &out, const Int x,
                            const std::index_sequence<I...>) { // {{{
    (void(out[I] = (x >> (CHAR_BIT * I)) & 0xff), ...);
} // }}}
#else
template <class T, class Int, std::size_t I>
constexpr void packed_copy_as_bytes_little(T &out, const Int x) { // {{{
    out[I] = (x >> (CHAR_BIT * I)) & 0xff;
} // }}}
template <class T, class Int>
constexpr void packed_copy_as_bytes_little(T &, const Int,
                                           const std::index_sequence<>) {}
template <class T, class Int, std::size_t I, std::size_t... Args>
constexpr void
packed_copy_as_bytes_little(T &out, const Int x,
                            const std::index_sequence<I, Args...>) { // {{{
    packed_copy_as_bytes_little<T, Int, I>(out, x);
    packed_copy_as_bytes_little(out, x, std::index_sequence<Args...>{});
} // }}}
#endif
} // namespace impl
template <class T, class Int>
constexpr void copy_as_bytes_little(T &out, const Int x) { // {{{
    my_static_assert(std::is_unsigned<Int>::value);
    my_static_assert(
        (sizeof(Int) <=
         std::conditional<std::is_array<T>::value, std::extent<T, 0>,
                          std::tuple_size<T>>::type::value));
    impl::packed_copy_as_bytes_little(out, x,
                                      std::make_index_sequence<sizeof(Int)>{});
} // }}}
namespace impl {
#if 0
template <class OutInt, class Vec, std::size_t... I>
constexpr void packed_copy_as_uint(OutInt &out, const Vec &v,
                                   const std::index_sequence<I...>) {
    out = ((OutInt(v[I]) << (CHAR_BIT * I)) + ...);
}
#else
template <class Vec, std::size_t I>
constexpr uint_fast64_t packed_copy_as_uint64(const Vec &v) {
    return uint_fast64_t(v[I]) << (CHAR_BIT * I);
}
template <class Vec>
constexpr uint_fast64_t packed_copy_as_uint64(const Vec &,
                                              const std::index_sequence<>) {
    return 0;
}
template <class Vec, std::size_t I, std::size_t... Args>
constexpr uint_fast64_t
packed_copy_as_uint64(const Vec &v, const std::index_sequence<I, Args...>) {
    return packed_copy_as_uint64<Vec, I>(v) +
           packed_copy_as_uint64(v, std::index_sequence<Args...>{});
}
template <class OutInt, class Vec, std::size_t... Args>
constexpr void packed_copy_as_uint(OutInt &out, const Vec &v,
                                   const std::index_sequence<Args...>) {
    out = packed_copy_as_uint64(v, std::index_sequence<Args...>{});
}
#endif
} // namespace impl
template <class OutInt, class T>
constexpr void copy_as_uint(OutInt &out, const T &v) {
    my_static_assert(std::is_unsigned<OutInt>::value);
    my_static_assert(
        (sizeof(OutInt) <=
         std::conditional_t<std::is_array<T>::value, std::extent<T, 0>,
                            std::tuple_size<T>>::value));
    impl::packed_copy_as_uint(out, v,
                              std::make_index_sequence<sizeof(OutInt)>{});
}
} // namespace tool
} // namespace drbg
// vim: set expandtab foldmethod=marker:
