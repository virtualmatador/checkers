#include "board.h"

const std::size_t main::Board::cell_count_;

main::Board::Board()
    : parent_{ nullptr }
    , score_{ 0 }
    , level_ { 0 }
{
}

main::Board::~Board()
{
}

bool main::Board::is_full(std::size_t index) const
{
    return fulls_.test(index);
}

bool main::Board::is_human(std::size_t index) const
{
    return humans_.test(index);
}

void main::Board::set_state(std::size_t index, std::size_t state)
{
    if (state == 0)
    {
        fulls_.set(index, false);
    }
    else
    {
        fulls_.set(index, true);
        humans_.set(index, state == 1);
    }
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
