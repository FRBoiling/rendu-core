#ifndef RENDU_TRAITS_CONSTNESS_H_
#define RENDU_TRAITS_CONSTNESS_H_

#include <type_traits>

namespace rendu::traits
{
    /**
     * @brief Transcribes the constness of a type to another type.
     * @tparam To The type to which to transcribe the constness.
     * @tparam From The type from which to transcribe the constness.
     */
    template<typename To, typename From>
    struct constness {
        /*! @brief The type resulting from the transcription of the constness. */
        using Type = std::remove_const_t<To>;
    };


    /*! @copydoc constness_as */
    template<typename To, typename From>
    struct constness<To, const From> {
        /*! @brief The type resulting from the transcription of the constness. */
        using Type = std::add_const_t<To>;
    };
}

#endif // RENDU_TRAITS_CONSTNESS_H_