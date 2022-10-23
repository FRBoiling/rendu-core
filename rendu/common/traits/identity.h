#ifndef RENDU_TRAITS_IDENTITY_H_
#define RENDU_TRAITS_IDENTITY_H_

namespace rendu::traits
{
    template <typename T>
    struct identity {
        using Type = T;
    };
}


#endif // RENDU_TRAITS_IDENTITY_H_