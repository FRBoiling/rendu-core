#ifndef RENDU_TYPES_STATEMACHINE_H_
#define RENDU_TYPES_STATEMACHINE_H_

#include <array>
#include <variant>
#include <traits/AnyOf.hpp>
#include <traits/Specialization.hpp>

namespace rendu::types
{
    class state_machine
    {
    public:
        struct NoState {};

        template <typename... T>
        using States = std::variant<NoState, T...>;

    public:
        state_machine() noexcept = delete;
        ~state_machine() noexcept = delete;
        state_machine(const state_machine&) = delete;
        state_machine(state_machine&&) noexcept = delete;
        state_machine& operator=(const state_machine&) = delete;
        state_machine& operator=(state_machine&&) noexcept = delete;

        template <typename State, typename... T, typename... Args>
        requires traits::AnyOf<State, T...> && std::is_constructible_v<State, Args...>
        static constexpr void ChangeState(States<T...>& fsm, [[maybe_unused]] Args&&... args) noexcept(noexcept(
            fsm.template emplace<State>(std::forward<Args>(args)...))) {
            fsm.template emplace<State>(std::forward<Args>(args)...);
        }

        template <typename State, typename... T>
        requires traits::AnyOf<State, T...>
        [[nodiscard]] static constexpr bool CurrentState(States<T...>& fsm) noexcept {
            return std::holds_alternative<State>(fsm);
        }

        template <typename... T>
        static constexpr void ResetState(States<T...>& fsm) noexcept {
            fsm.template emplace<NoState>();
        }

        template <typename Dispatcher, typename... T>
        static constexpr void Dispatch(States<T...>& fsm) {
            std::visit([&fsm](auto& state) {
                if constexpr(!std::is_same_v<std::remove_cvref_t<decltype(state)>, NoState>) {
                    static_assert(std::is_invocable_v<Dispatcher, decltype(fsm), decltype(state)>,
                        "Dispatcher does not support the operator() or state is not lvalue reference");
                    Dispatcher{}(fsm, state);
                }
            }, fsm);
        }
    };
}

#endif // RENDU_TYPES_STATEMACHINE_H_