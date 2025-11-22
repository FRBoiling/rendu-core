//
// Created by 沸腾 on 2025/11/22.
//

#ifndef RENDU_ITERATOR_PAIR_H
#define RENDU_ITERATOR_PAIR_H

#include "common/define.h"
#include <utility>
BEGIN_NAMESPACE_COMMON
    namespace Utils
    {
        /**
         * @class IteratorPair
         *
         * @brief Utility class to enable range for loop syntax for multimap.equal_range uses
         */
        template <class iterator, class end_iterator = iterator>
        class IteratorPair
        {
        public:
            constexpr IteratorPair() : _iterators()
            {
            }

            constexpr IteratorPair(iterator first, end_iterator second) : _iterators(first, second)
            {
            }

            constexpr IteratorPair(std::pair<iterator, end_iterator> iterators) : _iterators(iterators)
            {
            }

            constexpr iterator begin() const { return _iterators.first; }
            constexpr end_iterator end() const { return _iterators.second; }

        private:
            std::pair<iterator, end_iterator> _iterators;
        };

        namespace Containers
        {
            template <typename iterator, class end_iterator = iterator>
            constexpr IteratorPair<iterator, end_iterator> MakeIteratorPair(iterator first, end_iterator second)
            {
                return {first, second};
            }

            template <typename iterator, class end_iterator = iterator>
            constexpr IteratorPair<iterator, end_iterator> MakeIteratorPair(std::pair<iterator, end_iterator> iterators)
            {
                return iterators;
            }

            template <class M>
            auto MapEqualRange(M& map, typename M::key_type const& key)
            {
                return MakeIteratorPair(map.equal_range(key));
            }
        } // namespace Containers

    } // namespace Utils

END_NAMESPACE_COMMON

#endif //RENDU_ITERATOR_PAIR_H
