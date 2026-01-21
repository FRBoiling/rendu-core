//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_ENUM_UTILS_H
#define RENDU_ENUM_UTILS_H

#include "common/define.h"
#include "iterator_pair.h"
#include <iterator>
#include <string_view>

BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        struct EnumText
        {
            EnumText(char const* c, char const* t, char const* d) : Constant(c), Title(t), Description(d)
            {
            }

            // Enum constant of the value
            char const* const Constant;
            // Human-readable title of the value
            char const* const Title;
            // Human-readable description of the value
            char const* const Description;
        };

        namespace Impl::EnumUtilsImpl
        {
            template <typename Enum>
            struct EnumUtils
            {
                static size_t Count();
                static EnumText ToString(Enum value);
                static Enum FromIndex(size_t index);
                static size_t ToIndex(Enum index);
            };
        }

        class EnumUtils
        {
        public:
            template <typename Enum>
            static size_t Count() { return Impl::EnumUtilsImpl::EnumUtils<Enum>::Count(); }

            template <typename Enum>
            static EnumText ToString(Enum value)
            {
                return Impl::EnumUtilsImpl::EnumUtils<Enum>::ToString(value);
            }

            template <typename Enum>
            static Enum FromIndex(size_t index)
            {
                return Impl::EnumUtilsImpl::EnumUtils<Enum>::FromIndex(index);
            }

            template <typename Enum>
            static uint32 ToIndex(Enum value) { return Impl::EnumUtilsImpl::EnumUtils<Enum>::ToIndex(value); }

            template <typename Enum>
            static bool IsValid(Enum value)
            {
                try
                {
                    Impl::EnumUtilsImpl::EnumUtils<Enum>::ToIndex(value);
                    return true;
                }
                catch (...)
                {
                    return false;
                }
            }

            template <typename Enum>
            static bool IsValid(std::underlying_type_t<Enum> value) { return IsValid(static_cast<Enum>(value)); }

            template <typename Enum>
            class Iterator
            {
            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type = Enum;
                using pointer = Enum*;
                using reference = Enum&;
                using difference_type = std::ptrdiff_t;

                Iterator() : _index(EnumUtils::Count<Enum>())
                {
                }

                explicit Iterator(size_t index) : _index(index)
                {
                }

                bool operator==(const Iterator& other) const = default;
                std::strong_ordering operator<=>(const Iterator& other) const = default;

                difference_type operator-(Iterator const& other) const { return _index - other._index; }

                value_type operator[](difference_type d) const { return FromIndex<Enum>(_index + d); }
                value_type operator*() const { return operator[](0); }

                Iterator& operator+=(difference_type d)
                {
                    _index += d;
                    return *this;
                }

                Iterator& operator++() { return operator+=(1); }

                Iterator operator++(int)
                {
                    Iterator i = *this;
                    operator++();
                    return i;
                }

                Iterator operator+(difference_type d) const
                {
                    Iterator i = *this;
                    i += d;
                    return i;
                }

                Iterator& operator-=(difference_type d)
                {
                    _index -= d;
                    return *this;
                }

                Iterator& operator--() { return operator-=(1); }

                Iterator operator--(int)
                {
                    Iterator i = *this;
                    operator--();
                    return i;
                }

                Iterator operator-(difference_type d) const
                {
                    Iterator i = *this;
                    i -= d;
                    return i;
                }

            private:
                difference_type _index;
            };

            template <typename Enum>
            static Iterator<Enum> Begin() { return Iterator<Enum>(0); }

            template <typename Enum>
            static Iterator<Enum> End() { return Iterator<Enum>(); }

            template <typename Enum>
            static IteratorPair<Iterator<Enum>> Iterate() { return {Begin<Enum>(), End<Enum>()}; }

            template <typename Enum>
            static char const* ToConstant(Enum value) { return ToString(value).Constant; }

            template <typename Enum>
            static char const* ToTitle(Enum value) { return ToString(value).Title; }

            template <typename Enum>
            static char const* ToDescription(Enum value) { return ToString(value).Description; }
        };


        // 将枚举值转为整数（编译期/运行时均可）
        template <typename Enum>
        RC_COMMON_API constexpr auto EnumToInt(Enum e) noexcept -> std::underlying_type_t<Enum>
        {
            return static_cast<std::underlying_type_t<Enum>>(e); // static_cast 封装
        }



    } // namespace Utils

END_NAMESPACE_COMMON

#endif //RENDU_ENUM_UTILS_H
