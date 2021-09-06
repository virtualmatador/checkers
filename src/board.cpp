#include <cmath>

#include "board.h"

const std::size_t main::Board::cell_count_;
const std::size_t main::Board::last_row_;

main::Board::Board()
    : parent_{ nullptr }
    , score_{ 0.0 }
    , score_level_{ 0 }
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

template<bool killer>
void main::Board::list_options(std::list<Board>& boards,
    const std::size_t& piece, bool human)
{
    if (human || queens_.test(piece))
    {
        move<false, last_row_, 0, killer>(boards, piece, human);
        move<false, last_row_ - 1, last_row_ - 1, killer>(boards, piece, human);
    }
    if (!human || queens_.test(piece))
    {
        move<true, last_row_, last_row_ - 1, killer>(boards, piece, human);
        move<true, last_row_ - 1, 0, killer>(boards, piece, human);
    }
}

template<bool forward, std::size_t movement,
    std::size_t no_mod, bool killer>
void main::Board::move(
    std::list<Board>& boards, const std::size_t& piece, bool human)
{
    auto victum = piece;
    for (std::size_t cell = piece; [&]()
        {
            if constexpr(forward)
            {
                return cell <= cell_count_ - 1 - last_row_;
            }
            else
            {
                return cell >= last_row_;
            }
        }() && cell % (last_row_ * 2 - 1) != no_mod;)
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
            if constexpr(killer)
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
        (human && cell < last_row_) ||
        (!human && cell > cell_count_ - 1 - last_row_));
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
    score_level_ = level_;
    float human = 0.0f, cpu = 0.0f;
    for (std::size_t i = 0; i < cell_count_; ++i)
    {
        if (fulls_.test(i))
        {
            if (queens_.test(i))
            {
                if (humans_.test(i))
                {
                    human += 60.0f;
                }
                else
                {
                    cpu += 60.0f;
                }
            }
            else
            {
                if (humans_.test(i))
                {
                    human += std::pow(8.0 - get_row(i) + 16.0, 1.2);
                }
                else
                {
                    cpu += std::pow(get_row(i) + 16.0, 1.2);
                }
            }
        }
    }
    if (human == 0)
    {
        score_ = std::numeric_limits<float>::max();
    }
    else
    {
        score_ = cpu / human;
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

void main::Board::apply_score(const Board& board)
{
    if (board.is_human())
    {
        if (score_ < board.score_ || (score_ == board.score_ && (
            (score_level_ < board.score_level_ && score_ < 1.0) ||
            (score_level_ > board.score_level_ && score_ > 1.0))))
        {
            score_ = board.score_;
            score_level_ = board.score_level_;
        }
    }
    else
    {
        if (score_ > board.score_ || (score_ == board.score_ && (
            (score_level_ < board.score_level_ && score_ < 1.0) ||
            (score_level_ > board.score_level_ && score_ > 1.0))))
        {
            score_ = board.score_;
            score_level_ = board.score_level_;
        }
    }
}

std::size_t main::Board::get_row(std::size_t cell)
{
    return cell / (last_row_ * 2 - 1) * 2 + (
        cell % (last_row_ * 2 - 1)) / last_row_;
}

std::size_t main::Board::get_column(std::size_t cell)
{
    return cell % (last_row_ * 2 - 1) % last_row_;
}
