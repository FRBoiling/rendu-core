#ifndef RENDU_TYPES_UNIQUEINDEXER_H_
#define RENDU_TYPES_UNIQUEINDEXER_H_

#include <types/hash.h>
#include <platform/assert.h>

#include <algorithm>
#include <vector>

namespace rendu::types
{
    template <typename UniqueKey>
    class UniqueIndexer
    {
    public:
        using Hasher    = Hash<std::uint64_t>;
        using Storage   = std::vector<std::size_t>;

    public:
        UniqueIndexer() = default;
        ~UniqueIndexer() = default;
        UniqueIndexer(const UniqueIndexer&) = delete;
        UniqueIndexer(UniqueIndexer&&) noexcept = delete;
        UniqueIndexer& operator=(const UniqueIndexer&) = delete;
        UniqueIndexer& operator=(UniqueIndexer&&) noexcept = delete;

        template <typename T>
        [[nodiscard]] std::size_t Get() const {
            static const auto index = TypeIndexer<T>::GetIndex(m_Indexes);
            RENDU_ASSERT(index < m_Indexes->size(), "UniqueIndexer with same UniqueKey should not be in multiple instances!");
            return index;
        }

        [[nodiscard]] std::size_t Size() const noexcept {
            return m_Indexes->size();
        }

    private:
        template <typename T>
        struct TypeIndexer
        {
            [[nodiscard]] static std::size_t GetIndex(const std::unique_ptr<Storage>& storage)
            {
                if(const auto it = std::find(storage->cbegin(), storage->cend(), m_Key); it != storage->cend()) {
                    return std::distance(storage->cbegin(), it);
                }

                storage->emplace_back(m_Key);
                return storage->size() - 1uLL;
            }

            static constexpr auto m_Key = Hasher::template Get<T>();
        };

        std::unique_ptr<Storage> m_Indexes{std::make_unique<Storage>()};
    };

}

#endif // RENDU_TYPES_UNIQUEINDEXER_H_