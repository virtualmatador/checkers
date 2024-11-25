#ifndef SRC_DATA_H
#define SRC_DATA_H

#include <array>
#include <vector>

#include "board.h"

namespace main
{
    class Data
    {
        friend class Menu;
        friend class Game;

    public:
        Data();
        ~Data();
        void load();
        void save() const;
        void reset_all();
        void reset_game();
        void switch_sides();

    private:
        int difficulty_;
        bool alter_;
        bool rotate_;
        bool sound_;
        bool thumb_;
        int game_over_;
        Board board_;
    };

    extern Data data_;
}

#endif // SRC_DATA_H
