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
        static const std::size_t cell_count_ = 32;
        static const std::size_t last_row_ = 4;
        static const std::size_t piece_count_ = 11;
        static const std::size_t difficulty_limit_ = 9;
        static constexpr float win_score_ = 1000000.f;

    public:
        Board();
        ~Board();
        void list_options(
            std::list<Board>& boards, const std::size_t& piece, bool human);
        template<bool killer>
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
            std::size_t no_mod, bool killer>
        void move(
            std::list<Board>& boards, const std::size_t& piece, bool human);
        void add_option(std::list<Board>& boards, const std::size_t& piece,
            const std::size_t& victum, const std::size_t& cell, bool human);
        bool is_human() const;
    
    public:
        static std::size_t get_row(std::size_t cell);
        static std::size_t get_column(std::size_t cell);

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
