#ifndef SRC_BOARD_H
#define SRC_BOARD_H

#include <array>
#include <bitset>
#include <list>
#include <vector>

namespace main
{

    class Board
    {
    public:
        static const std::size_t cell_count_ = 41;
        static std::array<std::vector<std::size_t>, cell_count_> connections_;

    public:
        Board();
        ~Board();
        void list_options(
            std::list<Board>& boards, std::list<std::vector<int>>& moves) const;
        void list_options(
            std::list<Board>& boards) const;
        void evaluate();
        bool won() const;
        bool lost() const;

    private:
        std::bitset<cell_count_> fulls_;
        std::bitset<cell_count_> humans_;
        std::bitset<cell_count_> queens_;
        Board* parent_;
        short score_;
        unsigned short level_;

        friend class Data;
        friend class Game;
    };

}

#endif //SRC_BOARD_H
