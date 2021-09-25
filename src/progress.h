#ifndef PROGRESS_H
#define PROGRESS_H

#include <stdint.h>

namespace main
{
    enum class PROGRESS : std::int32_t
    {
        MENU,
        GAME,
    };

    extern PROGRESS progress_;
}

#endif //PROGRESS_H
