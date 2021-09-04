#include "board.h"

const std::size_t main::Board::cell_count_;

main::Board::Board()
    : parent_{ nullptr }
    , score_{ 0 }
    , level_ { 0 }
{
    sizeof(Board);
}

main::Board::~Board()
{
}

void main::Board::list_options(
    std::list<Board>& boards, std::list<std::vector<int>>& moves) const
{
    // TODO
}

void main::Board::list_options(
    std::list<Board>& boards) const
{
    // TODO
}


void main::Board::evaluate()
{
    // TODO
    score_ = 0;
}

bool main::Board::won() const
{
    for (std::size_t i = 0; i < cell_count_; ++i)
    {
        if (fulls_.test(i) && !humans_.test(i))
        {
            return false;
        }
    }
    return true;
}

bool main::Board::lost() const
{
    for (std::size_t i = 0; i < cell_count_; ++i)
    {
        if (fulls_.test(i) && humans_.test(i))
        {
            return false;
        }
    }
    return true;
}
