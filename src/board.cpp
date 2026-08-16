#include <cmath>

#include "board.h"

const std::size_t main::Board::cell_count_;
const std::size_t main::Board::last_row_;
const std::size_t main::Board::piece_count_;
const std::size_t main::Board::difficulty_limit_;
const std::size_t main::Board::max_moves_;
constexpr float main::Board::win_score_;

main::Board::Board()
    : parent_{ nullptr }
    , score_{ -1.0f }
    , score_level_{ 0 }
    , level_{ 0 }
    , kills_{ false }
    , traced_{ false }
{
}

main::Board::~Board()
{
}

std::list<main::Board> main::Board::list_options()
{
    kills_ = false;
    std::list<Board> options;
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        if (fulls_.test(i))
        {
            if (humans_.test(i) == is_human())
            {
                list_options<false>(options, i, is_human());
            }
        }
    }
    if (kills_)
    {
        for (auto it = options.begin(); it != options.end();)
        {
            if (!it->kills_)
            {
                it = options.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    return options;
}

bool main::Board::has_legal_move() const
{
    const bool human = is_human();
    for (std::size_t piece = 0; piece < cell_count_; ++piece)
    {
        if (!fulls_.test(piece) || humans_.test(piece) != human)
        {
            continue;
        }
        const bool queen = queens_.test(piece);
        if ((human || queen) && (
            has_move<false, last_row_, 0>(piece, human) ||
            has_move<false, last_row_ - 1, last_row_ - 1>(piece, human)))
        {
            return true;
        }
        if ((!human || queen) && (
            has_move<true, last_row_, last_row_ - 1>(piece, human) ||
            has_move<true, last_row_ - 1, 0>(piece, human)))
        {
            return true;
        }
    }
    return false;
}

template<bool killer>
void main::Board::list_options(std::list<Board>& boards,
    const std::size_t& piece, bool human)
{
    if (human || queens_.test(piece))
    {
        move<false, last_row_, 0, killer>(boards, piece, human);
        move<false, last_row_ - 1, last_row_ - 1, killer>(
            boards, piece, human);
    }
    if (!human || queens_.test(piece))
    {
        move<true, last_row_, last_row_ - 1, killer>(
            boards, piece, human);
        move<true, last_row_ - 1, 0, killer>(boards, piece, human);
    }
}

template<bool forward, std::size_t movement,
    std::size_t no_mod, bool killer>
void main::Board::move(std::list<Board>& boards,
    const std::size_t& piece, bool human)
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
                    traced_ = true;
                    add_option<true>(boards, piece, victum, cell, human);
                }
            }
            else
            {
                if (victum != piece)
                {
                    kills_ = true;
                    add_option<false>(boards, piece, victum, cell, human);
                }
                else if (!kills_)
                {
                    add_option<false>(boards, piece, victum, cell, human);
                }
            }
            if (!queens_.test(piece))
            {
                break;
            }
        }
    }
}

template<bool forward, std::size_t movement, std::size_t no_mod>
bool main::Board::has_move(const std::size_t& piece, bool human) const
{
    std::size_t cell = piece;
    const auto can_advance = [&]()
    {
        if constexpr(forward)
        {
            return cell <= cell_count_ - 1 - last_row_;
        }
        else
        {
            return cell >= last_row_;
        }
    };
    const auto advance = [&]()
    {
        if constexpr(forward)
        {
            cell += movement;
        }
        else
        {
            cell -= movement;
        }
    };

    if (!can_advance() || cell % (last_row_ * 2 - 1) == no_mod)
    {
        return false;
    }
    advance();
    if (!fulls_.test(cell))
    {
        return true;
    }
    if (humans_.test(cell) == human || !can_advance() ||
        cell % (last_row_ * 2 - 1) == no_mod)
    {
        return false;
    }
    advance();
    return !fulls_.test(cell);
}

template<bool killer>
void main::Board::add_option(std::list<Board>& boards, const std::size_t& piece,
    const std::size_t& victum, const std::size_t& cell, bool human)
{
    Board board;
    if constexpr(killer)
    {
        board = *this;
        board.parent_ = parent_;
    }
    else
    {
        board.fulls_ = fulls_;
        board.humans_ = humans_;
        board.queens_ = queens_;
        board.score_ = score_;
        board.score_level_ = score_level_;
        board.level_ = level_;
        board.parent_ = this;
        ++board.level_;
        if (!board.parent_->parent_)
        {
            board.moves_.emplace_back((unsigned char)piece);
        }
    }
    if (!board.parent_->parent_)
    {
        board.moves_.emplace_back((unsigned char)cell);
    }
    board.fulls_.set(piece, false);
    board.fulls_.set(cell, true);
    board.humans_.set(cell, human);
    board.queens_.set(cell, queens_.test(piece));
    board.kills_ = victum != piece;
    board.traced_ = false;
    bool convert = !board.queens_.test(cell) && ((human && cell < last_row_) ||
        (!human && cell > cell_count_ - 1 - last_row_));
    if (convert)
    {
        board.queens_.set(cell, true);
    }
    if (board.kills_)
    {
        board.fulls_.set(victum, false);
        if (!convert)
        {
            board.list_options<true>(boards, cell, human);
        }
    }
    if (!board.traced_)
    {
        boards.emplace_back(board);
    }
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
        score_ = win_score_;
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
            (score_level_ > board.score_level_ && score_ < 1.0) ||
            (score_level_ < board.score_level_ && score_ > 1.0))))
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
