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
        static const int human_queen_ = 5;
        static const int cpu_queen_ = 35;

    public:
        Board();
        ~Board();
        void list_options(
            std::list<Board>& boards, const std::size_t& piece, bool human);
        template<bool should_kill>
        void list_options(
            std::list<Board>& boards, const std::size_t& piece, bool human);
        void evaluate();
        bool won() const;
        bool lost() const;
        std::vector<std::size_t> trace_moves() const;
        void apply(const Board& board);
        void apply_score(const Board& board);
    
    private:
        template<bool forward, std::size_t movement,
            std::size_t no_mod, bool should_kill>
        void move(
            std::list<Board>& boards, const std::size_t& piece, bool human);
        void add_option(std::list<Board>& boards, const std::size_t& piece,
            const std::size_t& victum, const std::size_t& cell, bool human);
        bool is_human() const;

    private:
        std::bitset<cell_count_> fulls_;
        std::bitset<cell_count_> humans_;
        std::bitset<cell_count_> queens_;
        Board* parent_;
        float score_;
        unsigned short score_level_;
        unsigned short level_;

        friend class Data;
        friend class Game;
    };

}

#endif //SRC_BOARD_H
