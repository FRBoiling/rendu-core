#pragma once

#include <string>
#include <type_traits>
#include <cstdint>

namespace utils {

    template <typename T>
    std::string toHex(T value) {
        static_assert(std::is_unsigned_v<T>, "toHex requires unsigned integral type");

        constexpr size_t hexDigits = sizeof(T) * 2;
        constexpr const char* digits = "0123456789ABCDEF";

        std::string result;
        result.reserve(hexDigits);

        for (int i = static_cast<int>((hexDigits - 1) * 4); i >= 0; i -= 4) {
            uint8_t nibble = static_cast<uint8_t>((value >> i) & 0xF);
            result += digits[nibble];
        }

        return result;
    }

    // 常用类型的便捷重载
    inline std::string toHex(uint8_t value)  { return toHex<uint8_t>(value); }
    inline std::string toHex(uint16_t value) { return toHex<uint16_t>(value); }
    inline std::string toHex(uint32_t value) { return toHex<uint32_t>(value); }
    inline std::string toHex(uint64_t value) { return toHex<uint64_t>(value); }

} // namespace utils