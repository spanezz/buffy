#ifndef BUFFY_UTILS_CONSUMER_H
#define BUFFY_UTILS_CONSUMER_H

namespace buffy {

template<class ITEM>
class Consumer
{
public:
    virtual ~Consumer() {}
    virtual void consume(ITEM) = 0;
};

}

#endif
