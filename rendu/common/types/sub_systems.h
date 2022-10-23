#ifndef RENDU_TYPES_SUBSYSTEMS_H_
#define RENDU_TYPES_SUBSYSTEMS_H_

#include <rendu/Types/VectorAny.hpp>

namespace rendu::types
{
    template <typename System>
    requires std::is_class_v<System>
    class SubsystemDesign
    {
        struct UniqueKey {};

    public:
        SubsystemDesign() noexcept : m_Subsystems{} {};
        ~SubsystemDesign() noexcept = default;
        SubsystemDesign(const SubsystemDesign&) noexcept = delete;
        SubsystemDesign& operator=(const SubsystemDesign&) noexcept = delete;
        SubsystemDesign(SubsystemDesign&&) noexcept = delete;
        SubsystemDesign& operator=(SubsystemDesign&&) noexcept = delete;

        template <typename Subsystem, typename... Args>
        void RegisterSubsystem([[maybe_unused]] Args&&... args) {
            if(!HasSubsystem<Subsystem>()) {
                m_Subsystems.template Create<Subsystem>(std::forward<Args>(args)...);
            }
        }

        template <typename... Subsystems>
        [[nodiscard]] bool HasSubsystem() const {
            return m_Subsystems.template Has<Subsystems...>();
        }

        template <typename... Subsystems>
        [[nodiscard]] bool AnySubsystem() const {
            return m_Subsystems.template Any<Subsystems...>();
        }

        template <typename... Subsystems>
        [[nodiscard]] decltype(auto) GetSubsystem() {
            return m_Subsystems.template Get<Subsystems...>();
        }

        template <typename... Subsystems>
        [[nodiscard]] decltype(auto) GetSubsystem() const {
            return m_Subsystems.template Get<Subsystems...>();
        }

        template <typename... Subsystems>
        void RemoveSubsystem() {
            if constexpr(rendu::traits::Arguments<Subsystems...>::Single) {
                if(m_Subsystems.template Has<Subsystems...>()) {
                    m_Subsystems.template Remove<Subsystems...>();
                }
            } else {
                (RemoveSubsystem<Subsystems>(), ...);
            }
        }

    private:
        VectorAny<UniqueKey> m_Subsystems;
    };
}

#endif // RENDU_TYPES_SUBSYSTEMS_H_