#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#define PUT(x)                                                                 \
    do {                                                                       \
        std::cout << #x << "=" << (x) << std::endl;                            \
    } while (false)

namespace drbg {
namespace tool {
template<class... Args>
void unused(Args& ...arg) {
    ((void)(arg), ...);
}
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
} // namespace tool
} // namespace drbg
// vim: set expandtab foldmethod=marker:
