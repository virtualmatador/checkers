#include "board.h"

const std::size_t main::Board::cell_count_;
const int main::Board::human_queen_;
const int main::Board::cpu_queen_;

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
    std::list<Board>& boards, const std::size_t& piece, bool human)
{
    list_options<false>(boards, piece, human);
}

template<bool should_kill>
void main::Board::list_options(std::list<Board>& boards,
    const std::size_t& piece, bool human)
{
    if (human || queens_.test(piece))
    {
        move<false, 5, 0, should_kill>(boards, piece, human);
        move<false, 4, 4, should_kill>(boards, piece, human);
    }
    if (!human || queens_.test(piece))
    {
        move<true, 5, 4, should_kill>(boards, piece, human);
        move<true, 4, 0, should_kill>(boards, piece, human);
    }
}

template<bool forward, std::size_t movement,
    std::size_t no_mod, bool should_kill>
void main::Board::move(
    std::list<Board>& boards, const std::size_t& piece, bool human)
{
    auto victum = piece;
    for (std::size_t cell = piece; [&]()
        {
            if constexpr(forward)
            {
                return cell <= cpu_queen_;
            }
            else
            {
                return cell >= human_queen_;
            }
        }() && cell % 9 != no_mod;)
    {
        if constexpr(forward)
        {
            cell += movement;
        }
        else
        {
            cell -= movement;
        }
        if (fulls_.test(cell))
        {
            if (human == humans_.test(cell))
            {
                break;
            }
            else
            {
                if (victum == piece)
                {
                    victum = cell;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            if constexpr(should_kill)
            {
                if (victum != piece)
                {
                    add_option(boards, piece, victum, cell, human);
                }
            }
            else
            {
                add_option(boards, piece, victum, cell, human);
            }
            if (!queens_.test(piece))
            {
                break;
            }
        }
    }
}

void main::Board::add_option(std::list<Board>& boards, const std::size_t& piece,
    const std::size_t& victum, const std::size_t& cell, bool human)
{
    boards.emplace_back(*this);
    auto& board = boards.back();
    board.parent_ = this;
    board.fulls_.set(piece, false);
    board.fulls_.set(cell, true);
    board.humans_.set(cell, human);
    board.queens_.set(cell, queens_.test(piece) ||
        (human && cell < human_queen_) ||
        (!human && cell > cpu_queen_));
    if (victum != piece)
    {
        board.fulls_.set(victum, false);
        board.list_options<true>(boards, cell, human);
    }
    ++board.level_;
}

bool main::Board::is_human() const
{
    return level_ % 2 == 0;
}

void main::Board::evaluate()
{
    score_ = 0;
    for (std::size_t i = 0; i < cell_count_; ++i)
    {
        if (fulls_.test(i))
        {
            if (queens_.test(i))
            {
                if (humans_.test(i))
                {
                    score_ -= 30;
                }
                else
                {
                    score_ += 30;
                }
            }
            else
            {
                score_ += (i / 9 * 2 + (i % 9) / 5);
                if (humans_.test(i))
                {
                    score_ -= 20;
                }
                else
                {
                    score_ += 12;
                }
            }
        }
    }
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

std::vector<std::size_t> main::Board::trace_moves() const
{
    bool human = !is_human();
    std::vector<std::size_t> move;
    for (std::size_t i = 0; i < cell_count_; ++i)
    {
        if (fulls_.test(i))
        {
            if (!parent_->fulls_.test(i))
            {
                move.insert(move.begin(), i);
                break;
            }
        }
    }
    for (auto board = this;; board = board->parent_)
    {
        for (std::size_t i = 0; i < cell_count_; ++i)
        {
            if (!board->fulls_.test(i))
            {
                if (board->parent_->fulls_.test(i) &&
                    board->parent_->humans_.test(i) == human)
                {
                    move.insert(move.begin(), i);
                    break;
                }
            }
        }
        if (board->level_ != board->parent_->level_)
        {
            break;
        }
    }
    return move;
}

void main::Board::apply(const Board& board)
{
    fulls_ = board.fulls_;
    humans_ = board.humans_;
    queens_ = board.queens_;
}
