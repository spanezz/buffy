#ifndef BUFFY_MAILFOLDER_FROM_H
#define BUFFY_MAILFOLDER_FROM_H

#include <cstddef>

namespace buffy {
namespace mailfolder {

int is_from(const char *s, char *path = 0, size_t pathlen = 0);

}
}

#endif
