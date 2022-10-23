#ifndef RENDU_TYPES_VECTORANY_H_
#define RENDU_TYPES_VECTORANY_H_

#include <rendu/traits/Add.hpp>
#include <rendu/traits/Arguments.hpp>
#include <rendu/traits/NameOf.hpp>
#include <rendu/traits/Remove.hpp>
#include <rendu/traits/SameAs.hpp>
#include <rendu/Types/Any.hpp>
#include <rendu/Types/UniqueIndexer.hpp>

namespace rendu::types
{
    template <typename UniqueKey, std::size_t Capacity = sizeof(double)>
    class VectorAny final
    {
        using AnyType = types::Any<Capacity, alignof(std::aligned_storage_t < Capacity + !Capacity >)>;

    public:
        VectorAny() : m_TypeIndexer{}, m_Storage{} {}
        ~VectorAny() = default;
        VectorAny(const VectorAny&) = delete;
        VectorAny(VectorAny&&) noexcept = delete;
        VectorAny& operator=(const VectorAny&) = delete;
        VectorAny& operator=(VectorAny&&) noexcept = delete;

        template <typename T, typename... Args>
        void Create(Args&&... args)
        {
            static_assert(traits::SameAs<T, traits::RemoveCVRP<T>>, "Type is const/ptr/ref");

            const auto index = m_TypeIndexer.template Get<T>();
            if(index >= m_Storage.size()) {
                m_Storage.resize(index + 1);
            }

            RENDU_ASSERT(!m_Storage[index], "Type: {} already exist!", traits::NameOf<T>{});
            m_Storage[index].template Create<T>(std::forward<Args>(args)...);
        }

        template <typename... T>
        [[nodiscard]] bool Has() const
        {
            static_assert(!traits::Arguments<T...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<T, traits::RemoveCVRP<T>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<T...>::Single) {
                const auto index = m_TypeIndexer.template Get<T...>();
                return index < m_Storage.size() && m_Storage[index];
            } else {
                return (Has<T>() && ...);
            }
        }

        template <typename... T>
        [[nodiscard]] bool Any() const
        {
            static_assert(traits::Arguments<T...>::Size > 1, "Exclusion-only Type are not supported");
            static_assert((traits::SameAs<T, traits::RemoveCVRP<T>> && ...), "Type is const/ptr/ref");

            return (Has<T>() || ...);
        }

        template <typename... T>
        [[nodiscard]] decltype(auto) Get()
        {
            static_assert(!traits::Arguments<T...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<T, traits::RemoveCVRP<T>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<T...>::Single) {
                const auto index = m_TypeIndexer.template Get<T...>();

                RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Type: {} not exist!", traits::NameOf<T...>{});
                RENDU_ASSERT(AnyCast<T...>(&m_Storage[index]), "Type: {} type mismatch!", traits::NameOf<T...>{});

                return AnyCast<traits::AddRefLV<T>...>(m_Storage[index]);
            } else {
                return std::forward_as_tuple(Get<T>()...);
            }
        }

        template <typename... T>
        [[nodiscard]] decltype(auto) Get() const
        {
            static_assert(!traits::Arguments<T...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<T, traits::RemoveCVRP<T>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<T...>::Single) {
                const auto index = m_TypeIndexer.template Get<T...>();

                RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Type: {} not exist!", traits::NameOf<T...>{});
                RENDU_ASSERT(AnyCast<traits::AddConst<T>...>(&m_Storage[index]), "Type: {} type mismatch!", traits::NameOf<T...>{});

                return AnyCast<traits::AddCRLV<T>...>(m_Storage[index]);
            } else {
                return std::forward_as_tuple(Get<T>()...);
            }
        }

        template <typename... T>
        void Remove()
        {
            static_assert(!traits::Arguments<T...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<T, traits::RemoveCVRP<T>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<T...>::Single) {
                const auto index = m_TypeIndexer.template Get<T...>();
                RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Type: {} not exist!", traits::NameOf<T...>{});

                m_Storage[index].Reset();
            } else {
                (Remove<T>(), ...);
            }
        }

        void Clear() noexcept
        {
            for(auto& any : m_Storage) {
                any.Reset();
            }
        }

    private:
        types::UniqueIndexer<UniqueKey> m_TypeIndexer;
        std::vector<AnyType> m_Storage;
    };
}

#endif // RENDU_TYPES_VECTORANY_H_