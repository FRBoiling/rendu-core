#ifndef RENDU_TYPES_MODERNDESIGN_H_
#define RENDU_TYPES_MODERNDESIGN_H_

#include <rendu/Engine/Engine.hpp>

namespace rendu::types
{
    template <typename T>
    requires std::is_class_v<T>
    class modern_design
    {
    public:
        modern_design() noexcept = default;
        ~modern_design() noexcept = default;
        modern_design(const modern_design&) noexcept = delete;
        modern_design& operator=(const modern_design&) noexcept = delete;
        modern_design(modern_design&&) noexcept = delete;
        modern_design& operator=(modern_design&&) noexcept = delete;

    protected:
        [[nodiscard]] static T* CurrentSystem() {
            const auto result = rendu::Engine::HasSystem<T>();
            return result ? std::addressof(rendu::Engine::GetSystem<T>()) : nullptr;
        }
    };
}

#endif // RENDU_TYPES_MODERNDESIGN_H_