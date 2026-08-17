#ifndef SRC_DATA_H
#define SRC_DATA_H

#include <array>
#include <iosfwd>
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
        void load(std::istream& input);
        void save(std::ostream& output) const;
        void reset_all();
        void reset_game();
        void switch_sides();

    private:
        bool convert(int version, std::istream& input);

        static constexpr int save_version_ = 1;
        int difficulty_;
        bool alter_;
        bool rotate_;
        bool sound_;
        bool thumb_;
        bool highlight_;
        int game_over_;
        int last_move_;
        int previous_move_;
        Board board_;
        bool incompatible_save_;
        int incompatible_save_version_;
    };

    extern Data data_;
}

#endif // SRC_DATA_H
