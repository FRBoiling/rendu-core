#ifndef RENDU_TYPES_HASH_H_
#define RENDU_TYPES_HASH_H_

#include <traits/name_of.h>
#include <traits/FNV1a.h>
#include <traits/any_of.h>

namespace rendu::types
{
    template <typename>
    struct Hasher;

    template <typename>
    struct Equaler;

    template <typename T>
    requires traits::AnyOf<T, std::uint32_t, std::uint64_t>
    class Hash
    {
    public:
        using Hasher = traits::FNV1a<T>;
        using Value = T;

    public:
        Hash() = delete;
        Hash(const Hash&) = delete;
        Hash(Hash&&) noexcept = delete;
        Hash& operator=(const Hash&) = delete;
        Hash& operator=(Hash&&) noexcept = delete;

        template <typename Char>
        [[nodiscard]] static constexpr auto Get(std::basic_string_view<Char> str) noexcept
        {
            auto value{Hasher::Offset};
            for(std::size_t i = 0; i < str.size(); ++i) {
                value = (value ^ static_cast<T>(str[i])) * Hasher::Prime;
            }

            return value;
        }

        template <typename Type>
        [[nodiscard]] static constexpr auto Get() noexcept {
            return Get(rendu::traits::NameOf<Type>::Value);
        }
    };
}

#endif // RENDU_TYPES_HASH_H_