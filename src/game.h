#ifndef SRC_GAME_H
#define SRC_GAME_H

#include <atomic>
#include <thread>

#include "stage.h"

#include "board.h"
#include "data.h"

namespace main
{
    class Game: public core::Stage
    {
    public:
        Game();
        ~Game();
        void Escape() override;

    private:
        void update_view();
        void give_up();
        void game_over();
        void think();

    private:
        Board best_board_;
        std::thread thinker_;
        std::atomic<bool> stop_thinking_;
    };
}

#endif //SRC_GAME_H
