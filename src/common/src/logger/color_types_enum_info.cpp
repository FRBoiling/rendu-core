//
// Created by AI Assistant on 2025/1/20.
//

#include "common/utils/enum_utils.h"
#include <stdexcept>

#include "common/logger/appender_console.h"

BEGIN_NAMESPACE_COMMON
    namespace Utils::Impl::EnumUtilsImpl
    {
        template <>
        RC_API_EXPORT EnumText EnumUtils<ColorTypes>::ToString(ColorTypes value)
        {
            switch (value)
            {
            case ColorTypes::BLACK: return {"BLACK", "BLACK", ""};
            case ColorTypes::RED: return {"RED", "RED", ""};
            case ColorTypes::GREEN: return {"GREEN", "GREEN", ""};
            case ColorTypes::BROWN: return {"BROWN", "BROWN", ""};
            case ColorTypes::BLUE: return {"BLUE", "BLUE", ""};
            case ColorTypes::MAGENTA: return {"MAGENTA", "MAGENTA", ""};
            case ColorTypes::CYAN: return {"CYAN", "CYAN", ""};
            case ColorTypes::GREY: return {"GREY", "GREY", ""};
            case ColorTypes::YELLOW: return {"YELLOW", "YELLOW", ""};
            case ColorTypes::LRED: return {"LRED", "LRED", ""};
            case ColorTypes::LGREEN: return {"LGREEN", "LGREEN", ""};
            case ColorTypes::LBLUE: return {"LBLUE", "LBLUE", ""};
            case ColorTypes::LMAGENTA: return {"LMAGENTA", "LMAGENTA", ""};
            case ColorTypes::LCYAN: return {"LCYAN", "LCYAN", ""};
            case ColorTypes::WHITE: return {"WHITE", "WHITE", ""};
            default: throw std::out_of_range("value");
            }
        }

        template <>
        RC_API_EXPORT size_t EnumUtils<ColorTypes>::Count() { return 15; } // NUM_COLOR_TYPES之前的枚举值数量

        template <>
        RC_API_EXPORT ColorTypes EnumUtils<ColorTypes>::FromIndex(size_t index)
        {
            switch (index)
            {
            case 0: return ColorTypes::BLACK;
            case 1: return ColorTypes::RED;
            case 2: return ColorTypes::GREEN;
            case 3: return ColorTypes::BROWN;
            case 4: return ColorTypes::BLUE;
            case 5: return ColorTypes::MAGENTA;
            case 6: return ColorTypes::CYAN;
            case 7: return ColorTypes::GREY;
            case 8: return ColorTypes::YELLOW;
            case 9: return ColorTypes::LRED;
            case 10: return ColorTypes::LGREEN;
            case 11: return ColorTypes::LBLUE;
            case 12: return ColorTypes::LMAGENTA;
            case 13: return ColorTypes::LCYAN;
            case 14: return ColorTypes::WHITE;
            default: throw std::out_of_range("index");
            }
        }

        template <>
        RC_API_EXPORT size_t EnumUtils<ColorTypes>::ToIndex(ColorTypes value)
        {
            switch (value)
            {
            case ColorTypes::BLACK: return 0;
            case ColorTypes::RED: return 1;
            case ColorTypes::GREEN: return 2;
            case ColorTypes::BROWN: return 3;
            case ColorTypes::BLUE: return 4;
            case ColorTypes::MAGENTA: return 5;
            case ColorTypes::CYAN: return 6;
            case ColorTypes::GREY: return 7;
            case ColorTypes::YELLOW: return 8;
            case ColorTypes::LRED: return 9;
            case ColorTypes::LGREEN: return 10;
            case ColorTypes::LBLUE: return 11;
            case ColorTypes::LMAGENTA: return 12;
            case ColorTypes::LCYAN: return 13;
            case ColorTypes::WHITE: return 14;
            default: throw std::out_of_range("value");
            }
        }
    } // namespace Utils::Impl::EnumUtilsImpl

END_NAMESPACE_COMMON