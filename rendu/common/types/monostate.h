#ifndef RENDU_TYPES_MONOSTATE_H_
#define RENDU_TYPES_MONOSTATE_H_

#include <rendu/Types/FixedBuffer.hpp>

namespace rendu::types
{
    template <FixedBuffer Key>
    struct monostate
    {
        template <typename Type>
        void operator=(Type value) const noexcept {
            m_Data<Type> = value;
        }

        template <typename Type>
        operator Type() const noexcept {
            return m_Data<Type>;
        }

    private:
        template <typename Type>
        inline static Type m_Data = {};
    };
}

#endif // RENDU_TYPES_MONOSTATE_H_