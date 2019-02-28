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
template <class... Args> void unused(Args &... arg) { ((void)(arg), ...); }
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
template <class T, class Int, std::size_t... I>
constexpr void
packed_copy_as_bytes_little(T &out, const Int x,
                            const std::index_sequence<I...>) { // {{{
    (void(out[I] = (x >> (CHAR_BIT * I)) & 0xff), ...);
} // }}}
} // namespace impl
template <class T, class Int>
constexpr void copy_as_bytes_little(T &out, const Int x) { // {{{
    static_assert(std::is_unsigned_v<Int>);
    static_assert(sizeof(Int) <=
                  std::conditional_t<std::is_array_v<T>, std::extent<T, 0>,
                                     std::tuple_size<T>>::value);
    impl::packed_copy_as_bytes_little(out, x,
                                      std::make_index_sequence<sizeof(Int)>{});
} // }}}
namespace impl {
template <class OutInt, class Vec, std::size_t... I>
constexpr void packed_copy_as_uint(OutInt &out, const Vec &v,
                                   const std::index_sequence<I...>) {
    out = ((OutInt(v[I]) << (CHAR_BIT * I)) + ...);
}
} // namespace impl
template <class OutInt, class T>
constexpr void copy_as_uint(OutInt &out, const T &v) {
    static_assert(std::is_unsigned_v<OutInt>);
    static_assert(sizeof(OutInt) <=
                  std::conditional_t<std::is_array_v<T>, std::extent<T, 0>,
                                     std::tuple_size<T>>::value);
    impl::packed_copy_as_uint(out, v,
                              std::make_index_sequence<sizeof(OutInt)>{});
}
} // namespace tool
} // namespace drbg
// vim: set expandtab foldmethod=marker: