#ifndef BUFFY_UTILS_CONSUMER_H
#define BUFFY_UTILS_CONSUMER_H

/*
 * Consumer and filter interfaces
 *
 * Copyright (C) 2003--2008  Enrico Zini <enrico@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace buffy {

template<class ITEM>
class Consumer
{
public:
    virtual ~Consumer() {}
    virtual void consume(ITEM) = 0;
};

template<class ITEM>
class Matcher
{
public:
    virtual ~Matcher() {}
    virtual bool match(ITEM& item) const
    {
        return true;
    }
};

template<class ITEM>
class Filter : public Consumer<ITEM>
{
protected:
    Consumer<ITEM>& next;
public:
    Filter<ITEM>(Consumer<ITEM>& next) : next(next) {}

    void consume(ITEM item) override
    {
        next.consume(item);
    }
};

template<class ITEM>
class MatcherFilter : public Filter<ITEM>
{
protected:
    Matcher<ITEM>& matcher;

public:
    MatcherFilter<ITEM>(Matcher<ITEM>& matcher, Consumer<ITEM>& next) throw ()
        : Filter<ITEM>(next), matcher(matcher) {}

    void consume(ITEM item) override
    {
        if (matcher.match(item))
            this->next.consume(item);
    }
};

}

// vim:set ts=4 sw=4:
#endif
