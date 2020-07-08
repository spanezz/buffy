#ifndef BUFFY_UTILS_SMART_POINTER_H
#define BUFFY_UTILS_SMART_POINTER_H

/*
 * Copyright (C) 2004--2008  Enrico Zini <enrico@enricozini.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

namespace buffy {

template<class ITEM>
class SmartPointer
{
protected:
    ITEM* impl;

public:
    SmartPointer() throw () : impl(0) {}

    SmartPointer(const SmartPointer& sp) throw ()
    {
        if (sp.impl)
            sp.impl->ref();
        impl = sp.impl;
    }

    SmartPointer(ITEM* otherimpl) throw ()
    {
        if (otherimpl)
            otherimpl->ref();
        impl = otherimpl;
    }

    ~SmartPointer() throw ()
    {
        if (impl && impl->unref())
            delete impl;
    }

    SmartPointer& operator=(const SmartPointer& sp) throw ()
    {
        if (sp.impl)
            sp.impl->ref();  // Do it early to correctly handle the case of x = x;
        if (impl && impl->unref())
            delete impl;
        impl = sp.impl;
        return *this;
    }

    operator bool() const throw () { return impl != 0; }
};

class SmartPointerItem
{
protected:
    int _ref;

public:
    SmartPointerItem() throw () : _ref(0) {}
    
    /// Increment the reference count for this object
    void ref() throw () { ++_ref; }

    /// Decrement the reference count for this object, returning true when it
    /// reaches 0
    bool unref() throw () { return --_ref == 0; }
};

}

// vim:set ts=3 sw=3:
#endif
