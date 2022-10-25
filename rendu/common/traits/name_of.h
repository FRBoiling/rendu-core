#ifndef RENDU_TRAITS_NAMEOF_H_
#define RENDU_TRAITS_NAMEOF_H_

#include <array>
#include <string_view>

namespace rendu::traits
{
    template <typename T>
    class NameOf final {
        template <std::size_t... Indexes>
        [[nodiscard]] static constexpr auto Substring(std::string_view str, std::index_sequence<Indexes...>) noexcept {
            return std::array{str[Indexes]..., '\0'};
        }

        template <typename>
        [[nodiscard]] static constexpr auto ParseName() noexcept
        {
        #if defined(RENDU_COMPILER_CLANG)
            constexpr auto prefix   = std::string_view{"[T = "};
            constexpr auto suffix   = std::string_view{"]"};
            constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
        #elif defined(RENDU_COMPILER_GCC)
            constexpr auto prefix   = std::string_view{"; T = "};
            constexpr auto suffix   = std::string_view{"]"};
            constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
        #elif defined(RENDU_COMPILER_MSVC)
            constexpr auto prefix   = std::string_view{"ParseName<"};
            constexpr auto suffix   = std::string_view{">(void)"};
            constexpr auto function = std::string_view{__FUNCSIG__};
        #else
            # error Unsupported compiler
        #endif

            constexpr auto start = function.find(prefix) + prefix.size();
            constexpr auto end = function.rfind(suffix);
            static_assert(start < end);
            constexpr auto name = function.substr(start, (end - start));
            return Substring(name, std::make_index_sequence<name.size()>{});
        }

        template <typename Type>
        static constexpr auto NameStorage = ParseName<T>();

        template <typename Type>
        [[nodiscard]] static constexpr auto Name() {
            return std::string_view{NameStorage<Type>.data()};
        }

    public:
        static constexpr auto Value = Name<T>();

        constexpr operator const char*() const noexcept {
            return Value.data();
        }

        constexpr operator std::string_view() const noexcept {
            return Value;
        }
    };
}

#endif // RENDU_TRAITS_NAMEOF_H_