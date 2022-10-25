#ifndef RENDU_TYPES_REFERENCEPOINTER_H_
#define RENDU_TYPES_REFERENCEPOINTER_H_

#include <optional>
#include <platform/Assert.hpp>

namespace rendu::types
{
    template <typename T>
    class reference_pointer
    {
        struct Container {
            std::optional<T> m_Storage;
            std::size_t m_Refs;
        };

    public:
        reference_pointer() : m_Container{} {};

        template <typename... Args>
        reference_pointer(std::in_place_t, Args&&... args) : m_Container{new (std::nothrow) Container} {
            if(m_Container) {
                m_Container->m_Storage = std::make_optional<T>(std::forward<Args>(args)...);
                m_Container->m_Refs = 1uLL;
            }
        }

        ~reference_pointer() {
            Reset();
        }

        reference_pointer(const reference_pointer& other) : m_Container{other.m_Container} {
            if(m_Container) {
                ++m_Container->m_Refs;
            }
        }

        reference_pointer(reference_pointer&& other) noexcept : m_Container{other.m_Container} {
            other.m_Container = nullptr;
        }

        reference_pointer& operator=(const reference_pointer& other)
        {
            Reset();

            if(other.m_Container) {
                m_Container = other.m_Container;
                ++m_Container->m_Refs;
            }

            return *this;
        }

        reference_pointer& operator=(reference_pointer&& other) noexcept
        {
            Reset();

            if(other.m_Container) {
                m_Container = other.m_Container;
                other.m_Container = nullptr;
            }

            return *this;
        }

        template <typename... Args>
        [[nodiscard]] static reference_pointer Create([[maybe_unused]] Args&&... args) noexcept {
            return reference_pointer{std::in_place, std::forward<Args>(args)...};
        }

        [[nodiscard]] T& Ref() const {
            RENDU_ASSERT(m_Container, "Container is empty!");
            return m_Container->m_Storage.value();
        }

        [[nodiscard]] T* Ptr() const {
            return m_Container ? &m_Container->m_Storage.value() : nullptr;
        }

        [[nodiscard]] bool Shared() const noexcept {
            RENDU_ASSERT(m_Container, "Container is empty!");
            return m_Container && m_Container->m_Refs > 1;
        }

        [[nodiscard]] std::size_t Count() const noexcept {
            RENDU_ASSERT(m_Container, "Container is empty!");
            return m_Container->m_Refs;
        }

        void Reset() noexcept
        {
            if(m_Container && !--m_Container->m_Refs) {
                m_Container->m_Storage.reset();
                delete m_Container; m_Container = nullptr;
            }
        }

        [[nodiscard]] operator bool() const noexcept {
            return m_Container;
        }

        [[nodiscard]] T& operator*() const {
            RENDU_ASSERT(m_Container);
            return m_Container->m_Storage.value();
        }

    private:
        Container* m_Container;
    };
}

#endif // RENDU_TYPES_REFERENCEPOINTER_H_