#ifndef RENDU_TYPES_VECTORKVANY_H_
#define RENDU_TYPES_VECTORKVANY_H_

#include <traits/Add.hpp>
#include <traits/Arguments.hpp>
#include <traits/NameOf.hpp>
#include <traits/Remove.hpp>
#include <traits/SameAs.hpp>
#include <Types/Any.hpp>
#include <Types/UniqueIndexer.hpp>

namespace rendu::types
{
    template <typename UniqueKey, std::size_t Capacity = sizeof(double)>
    class VectorKVAny final
    {
        using AnyType = types::Any<Capacity, alignof(std::aligned_storage_t < Capacity + !Capacity >)>;

    public:
        VectorKVAny() : m_TypeIndexer{}, m_Storage{} {}
        ~VectorKVAny() = default;
        VectorKVAny(const VectorKVAny&) = delete;
        VectorKVAny(VectorKVAny&&) noexcept = delete;
        VectorKVAny& operator=(const VectorKVAny&) = delete;
        VectorKVAny& operator=(VectorKVAny&&) noexcept = delete;

        template <typename Key, typename T, typename... Args>
        void Create(Args&&... args)
        {
            static_assert(traits::SameAs<Key, traits::RemoveCVRP<Key>>, "Type is const/ptr/ref");
            static_assert(traits::SameAs<T, traits::RemoveCVRP<T>>, "Type is const/ptr/ref");

            const auto index = m_TypeIndexer.template Get<Key>();
            if(index >= m_Storage.size()) {
                m_Storage.resize(index + 1u);
            }

            RENDU_ASSERT(!m_Storage[index], "Key: {}, Type: {} already exist!", traits::NameOf<Key>{}, traits::NameOf<T>{});
            m_Storage[index].template Create<T>(std::forward<Args>(args)...);
        }

        template <typename... Key>
        [[nodiscard]] bool Has() const
        {
            static_assert(!traits::Arguments<Key...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<Key, traits::RemoveCVRP<Key>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<Key...>::Single) {
                const auto index = m_TypeIndexer.template Get<Key...>();
                return index < m_Storage.size() && m_Storage[index];
            } else {
                return (Has<Key>() && ...);
            }
        }

        template <typename... Key>
        [[nodiscard]] bool Any() const
        {
            static_assert(traits::Arguments<Key...>::Size > 1, "Exclusion-only Type are not supported");
            static_assert((traits::SameAs<Key, traits::RemoveCVRP<Key>> && ...), "Type is const/ptr/ref");

            return (Has<Key>() || ...);
        }

        template <typename Key, typename T>
        [[nodiscard]] decltype(auto) Get()
        {
            static_assert(traits::SameAs<Key, traits::RemoveCVRP<Key>>, "Type is const/ptr/ref");
            static_assert(traits::SameAs<T, traits::RemoveCVRP<T>>, "Type is const/ptr/ref");

            const auto index = m_TypeIndexer.template Get<Key>();

            RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Key: {}, Type: {} not exist!",
                traits::NameOf<Key>{}, traits::NameOf<T>{});
            RENDU_ASSERT(AnyCast<T>(&m_Storage[index]), "Key: {}, Type: {} type mismatch!",
                traits::NameOf<Key>{}, traits::NameOf<T>{});

            return AnyCast<T&>(m_Storage[index]);
        }

        template <typename Key, typename T>
        [[nodiscard]] decltype(auto) Get() const
        {
            static_assert(traits::SameAs<Key, traits::RemoveCVRP<Key>>, "Type is const/ptr/ref");
            static_assert(traits::SameAs<T, traits::RemoveCVRP<T>>, "Type is const/ptr/ref");

            const auto index = m_TypeIndexer.template Get<Key>();

            RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Key: {}, Type: {} not exist!",
                traits::NameOf<Key>{}, traits::NameOf<T>{});
            RENDU_ASSERT(AnyCast<traits::AddConst<T>>(&m_Storage[index]), "Key: {}, Type: {} type mismatch!",
                traits::NameOf<Key>{}, traits::NameOf<T>{});

            return AnyCast<traits::AddCRLV<T>>(m_Storage[index]);
        }

        template <typename... Key>
        void Remove()
        {
            static_assert(!traits::Arguments<Key...>::Orphan, "Pack is empty!");
            static_assert((traits::SameAs<Key, traits::RemoveCVRP<Key>> && ...), "Type is const/ptr/ref");

            if constexpr(traits::Arguments<Key...>::Single) {
                const auto index = m_TypeIndexer.template Get<Key...>();

                RENDU_ASSERT(index < m_Storage.size() && m_Storage[index], "Key: {} not exist!", traits::NameOf<Key...>{});

                m_Storage[index].Reset();
            } else {
                (Remove<Key>(), ...);
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

#endif // RENDU_TYPES_VECTORKVANY_H_