/*
* Created by boil on 2023/4/9.
*/

#ifndef RENDU_BINDING_CHECK_H
#define RENDU_BINDING_CHECK_H

#include <entt/entt.hpp>
#include "state.h"

namespace input {

    namespace details {
        class binding_check {
        public:
            binding_check() = default;

            virtual ~binding_check() = default;

            virtual bool check(entt::registry &registry, const state &state) const {
                return false;
            }
        };

        class binding_and_combinator final : public binding_check {
        private:
            std::unique_ptr<binding_check> m_a;
            std::unique_ptr<binding_check> m_b;

        public:
            binding_and_combinator(
                    std::unique_ptr<binding_check> a,
                    std::unique_ptr<binding_check> b
            ) : m_a(std::move(a)), m_b(std::move(b)) {}

            bool check(entt::registry &registry, const state &state) const override {
                return m_a->check(registry, state) && m_b->check(registry, state);
            }
        };

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
    };

    using binding_type = std::unique_ptr<details::binding_check>;

    binding_type nobinding() {
        return std::make_unique<details::binding_check>();
    }

}

input::binding_type operator|(
        input::binding_type a,
        input::binding_type b
) {
    return std::make_unique<input::details::binding_or_combinator>(
            std::move(a),
            std::move(b)
    );
}

input::binding_type operator&(
        input::binding_type a,
        input::binding_type b
) {
    return std::make_unique<input::details::binding_and_combinator>(
            std::move(a),
            std::move(b)
    );
}

#endif //RENDU_BINDING_CHECK_H
