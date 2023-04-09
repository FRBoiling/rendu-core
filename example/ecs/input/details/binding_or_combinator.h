/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_BINDING_OR_COMBINATOR_H
#define RENDU_BINDING_OR_COMBINATOR_H

#include "binding_check.h"

namespace input {
    namespace details {
        class binding_or_combinator final : public binding_check {
        private:
            std::unique_ptr<binding_check> m_a;
            std::unique_ptr<binding_check> m_b;

        public:
            binding_or_combinator(
                    std::unique_ptr<binding_check> a,
                    std::unique_ptr<binding_check> b
            ) : m_a(std::move(a)), m_b(std::move(b)) {}

            bool check(entt::registry &registry, const state &state) const override {
                return m_a->check(registry, state) || m_b->check(registry, state);
            }
        };
    }
}
#endif //RENDU_BINDING_OR_COMBINATOR_H
