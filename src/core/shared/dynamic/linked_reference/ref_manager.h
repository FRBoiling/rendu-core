/*
* Created by boil on 2022/10/20.
*/

#ifndef RENDU_REFMANAGER_H_
#define RENDU_REFMANAGER_H_

#include "../linked_list.h"
#include "../linked_reference/reference.h"

template <class TO, class FROM>
class RefManager : public LinkedListHead
{
public:
    typedef LinkedListHead::Iterator<Reference<TO, FROM>> iterator;
    typedef LinkedListHead::Iterator<Reference<TO, FROM> const> const_iterator;
    RefManager() { }

    Reference<TO, FROM>* getFirst() { return static_cast<Reference<TO, FROM>*>(LinkedListHead::getFirst()); }

    Reference<TO, FROM> const* getFirst() const { return static_cast<Reference<TO, FROM> const*>(LinkedListHead::getFirst()); }

    iterator begin() { return iterator(getFirst()); }
    iterator end() { return iterator(nullptr); }

    const_iterator begin() const { return const_iterator(getFirst()); }
    const_iterator end() const { return const_iterator(nullptr); }

    virtual ~RefManager()
    {
        clearReferences();
    }

    void clearReferences()
    {
        while (Reference<TO, FROM>* ref = getFirst())
            ref->invalidate();
    }
};

#endif
