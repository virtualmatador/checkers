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
        void FeedUri(const char* uri, std::function<void(
            const std::vector<unsigned char>&)>&& consume) override;

    private:
        void play_audio(const char* audio);
        void reset_board();
        void set_preferences();
        void validate_move();
        void update_view();
        void move_human();
        void move_cpu();
        void loose();
        void draw();
        void win();
        void game_over();
        void join_threads();
        void think();
        void guess();

    private:
        Board best_board_;
        std::list<Board> boards_;
        std::thread thinker_;
        std::thread guesser_;
        std::atomic<bool> stop_thinking_;
    };
}

#endif //SRC_GAME_H
