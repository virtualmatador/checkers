#include "data.h"

#include <iomanip>
#include <ios>
#include <istream>
#include <ostream>

main::Data main::data_;

main::Data::Data()
    : incompatible_save_{ false }
    , incompatible_save_version_{ save_version_ }
{
}

main::Data::~Data()
{
}

void main::Data::load(std::istream& input)
{
    input >> std::dec >> std::noboolalpha >> std::skipws;
    int version = 0;
    if (!(input >> version))
    {
        reset_all();
        return;
    }

    if (version != save_version_)
    {
        if (convert(version, input))
        {
            incompatible_save_ = false;
            incompatible_save_version_ = save_version_;
            return;
        }

        reset_all();
        incompatible_save_ = true;
        incompatible_save_version_ = version;
        return;
    }

    Data loaded;
    int moves_count = 0;
    bool valid = static_cast<bool>(
        input >> loaded.difficulty_
              >> loaded.alter_
              >> loaded.rotate_
              >> loaded.sound_
              >> loaded.thumb_
              >> loaded.highlight_
              >> loaded.game_over_
              >> loaded.last_move_
              >> loaded.previous_move_
              >> moves_count);

    valid = valid && moves_count >= 0 &&
        moves_count <= static_cast<int>(Board::max_moves_);
    for (int i = 0; valid && i < moves_count; ++i)
    {
        int move = 0;
        valid = static_cast<bool>(input >> move) && move >= 0 &&
            move < static_cast<int>(Board::cell_count_);
        if (valid)
        {
            loaded.board_.moves_.emplace_back(
                static_cast<unsigned char>(move));
        }
    }

    int level = 0;
    if (valid)
    {
        valid = static_cast<bool>(input >> level) && level >= 0 && level < 2;
        if (valid)
        {
            loaded.board_.level_ = static_cast<unsigned char>(level);
        }
    }

    for (std::size_t i = 0; valid && i < Board::cell_count_; ++i)
    {
        bool value = false;
        valid = static_cast<bool>(input >> value);
        if (valid)
        {
            loaded.board_.fulls_.set(i, value);
        }
    }
    for (std::size_t i = 0; valid && i < Board::cell_count_; ++i)
    {
        bool value = false;
        valid = static_cast<bool>(input >> value);
        if (valid)
        {
            loaded.board_.humans_.set(i, value);
        }
    }
    for (std::size_t i = 0; valid && i < Board::cell_count_; ++i)
    {
        bool value = false;
        valid = static_cast<bool>(input >> value);
        if (valid)
        {
            loaded.board_.queens_.set(i, value);
        }
    }

    if (valid)
    {
        input >> std::ws;
        valid = input.eof() && !input.bad();
    }

    valid = valid && loaded.difficulty_ >= 1 &&
        loaded.difficulty_ < static_cast<int>(Board::difficulty_limit_) &&
        loaded.game_over_ >= 0 && loaded.game_over_ < 4 &&
        loaded.last_move_ >= -1 &&
        loaded.last_move_ < static_cast<int>(Board::cell_count_) &&
        loaded.previous_move_ >= -1 &&
        loaded.previous_move_ < static_cast<int>(Board::cell_count_);
    if (!valid)
    {
        reset_all();
        return;
    }

    difficulty_ = loaded.difficulty_;
    alter_ = loaded.alter_;
    rotate_ = loaded.rotate_;
    sound_ = loaded.sound_;
    thumb_ = loaded.thumb_;
    highlight_ = loaded.highlight_;
    game_over_ = loaded.game_over_;
    last_move_ = loaded.last_move_;
    previous_move_ = loaded.previous_move_;
    board_ = loaded.board_;
    incompatible_save_ = false;
    incompatible_save_version_ = save_version_;
}

bool main::Data::convert(int version, std::istream&)
{
    switch (version)
    {
    default:
        return false;
    }
}

void main::Data::save(std::ostream& output) const
{
    if (incompatible_save_)
    {
        output.setstate(std::ios::failbit);
        return;
    }

    output << std::dec << std::noboolalpha << std::noshowbase <<
        std::noshowpos;
    output.width(0);
    output << save_version_ << '\n'
           << difficulty_ << '\n'
           << alter_ << '\n'
           << rotate_ << '\n'
           << sound_ << '\n'
           << thumb_ << '\n'
           << highlight_ << '\n'
           << game_over_ << '\n'
           << last_move_ << '\n'
           << previous_move_ << '\n'
           << static_cast<unsigned int>(board_.moves_.size()) << '\n';
    for (const auto move : board_.moves_)
    {
        output << static_cast<unsigned int>(move) << '\n';
    }
    output << static_cast<unsigned int>(board_.level_) << '\n';
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        output << board_.fulls_.test(i) << '\n';
    }
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        output << board_.humans_.test(i) << '\n';
    }
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        output << board_.queens_.test(i) << '\n';
    }
}

void main::Data::reset_all()
{
    incompatible_save_ = false;
    incompatible_save_version_ = save_version_;
    difficulty_ = 2;
    alter_ = false;
    rotate_ = false;
    sound_ = false;
    thumb_ = false;
    highlight_ = false;
    reset_game();
}

void main::Data::reset_game()
{
    game_over_ = 0;
    last_move_ = -1;
    previous_move_ = -1;
    board_.moves_.clear();
    board_.level_ = 0;
    board_.score_ = -1.0f;
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        board_.fulls_.set(i, i < Board::piece_count_ ||
                                 i > Board::cell_count_ - 1 - Board::piece_count_);
        board_.humans_.set(i,
                           i > Board::cell_count_ - 1 - Board::piece_count_);
        board_.queens_.set(i, false);
    }
}

void main::Data::switch_sides()
{
    rotate_ = !rotate_;
    alter_ = !alter_;
    if (game_over_ == 1)
    {
        game_over_ = 2;
    }
    else if (game_over_ == 2)
    {
        game_over_ = 1;
    }
    for (auto &move : board_.moves_)
    {
        move = Board::cell_count_ - 1 - move;
    }
    if (last_move_ >= 0)
    {
        last_move_ = Board::cell_count_ - 1 - last_move_;
    }
    if (previous_move_ >= 0)
    {
        previous_move_ = Board::cell_count_ - 1 - previous_move_;
    }
    if (board_.level_ == 0)
    {
        board_.level_ = 1;
    }
    else if (board_.level_ == 1)
    {
        board_.level_ = 0;
    }
    board_.score_ = -1.0f;
    auto fulls = board_.fulls_;
    auto humans = board_.humans_;
    auto queens = board_.queens_;
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        board_.fulls_.set(i, fulls.test(Board::cell_count_ - 1 - i));
        board_.humans_.set(i, !humans.test(Board::cell_count_ - 1 - i));
        board_.queens_.set(i, queens.test(Board::cell_count_ - 1 - i));
    }
}
