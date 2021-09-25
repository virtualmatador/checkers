#ifndef SRC_MENU_H
#define SRC_MENU_H

#include "stage.h"

namespace main
{
    class Menu: public core::Stage
    {
    public:
        Menu();
        ~Menu();
        void Escape() override;
        void FeedUri(const char* uri, std::function<void(
            const std::vector<unsigned char>&)>&& consume) override;

    private:
        void Play();
    };
}

#endif //SRC_MENU_H
