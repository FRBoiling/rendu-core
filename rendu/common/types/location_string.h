#ifndef RENDU_TYPES_LOCATIONSTRING_H_
#define RENDU_TYPES_LOCATIONSTRING_H_

#include <types/source_location.h>

#include <concepts>

namespace rendu::types
{
    // TODO: MSVC bad optimize source location, need trick
    struct location_string
    {
        constexpr location_string(const SourceLocation location = SourceLocation::Create()) noexcept
            : m_Location{location}, m_Msg{} {}

        template <std::convertible_to<std::string_view> T>
        constexpr location_string(T&& msg, const SourceLocation location = SourceLocation::Create()) noexcept
            : m_Location{location}, m_Msg{msg} {}

        types::SourceLocation m_Location;
        std::string_view m_Msg;
    };
}

#endif // RENDU_TYPES_LOCATIONSTRING_H_