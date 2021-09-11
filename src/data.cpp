#include <iostream>
#include <sstream>

#include "bridge.h"

#include "data.h"

#include "toolbox.hpp"

main::Data main::data_;

main::Data::Data()
{
}

main::Data::~Data()
{
}

void main::Data::load()
{
    try
    {
        toolbox::Load("OPTION_DIFFICULTY", difficulty_,
            0, (int)Board::difficulty_limit_);
        toolbox::Load("OPTION_ALTER", alter_, false, false);
        toolbox::Load("OPTION_ROTATE", rotate_, false, false);
        toolbox::Load("OPTION_SOUND", sound_, false, false);
        toolbox::Load("GAME_OVER", game_over_, 0, 4);
        int moves_count;
        toolbox::Load("GAME_MOVES_COUNT", moves_count,
            0, (int)Board::cell_count_ / 2);
        board_.moves_.clear();
        for (std::size_t i = 0; i < moves_count; ++i)
        {
            std::ostringstream composer;
            int buffer;
            composer << "GAME_MOVES_" << i;
            toolbox::Load(composer.str().c_str(), buffer,
                0, (int)Board::cell_count_);
            board_.moves_.emplace_back((unsigned char)buffer);
        }
        int level;
        toolbox::Load("GAME_LEVEL", level, 0, 2);
        board_.level_ = (unsigned char)level;
        for (std::size_t i = 0; i < board_.fulls_.size(); ++i)
        {
            std::ostringstream composer;
            bool buffer;
            composer << "GAME_FULL_" << i;
            toolbox::Load(composer.str().c_str(), buffer, false, false);
            board_.fulls_.set(i, buffer);
        }
        for (std::size_t i = 0; i < board_.humans_.size(); ++i)
        {
            std::ostringstream composer;
            bool buffer;
            composer << "GAME_HUMAN_" << i;
            toolbox::Load(composer.str().c_str(), buffer, false, false);
            board_.humans_.set(i, buffer);
        }
        for (std::size_t i = 0; i < board_.queens_.size(); ++i)
        {
            std::ostringstream composer;
            bool buffer;
            composer << "GAME_QUEEN_" << i;
            toolbox::Load(composer.str().c_str(), buffer, false, false);
            board_.queens_.set(i, buffer);
        }
    }
    catch(...)
    {
        reset_all();
    }
}

void main::Data::save() const
{
    toolbox::Save("OPTION_DIFFICULTY", difficulty_);
    toolbox::Save("OPTION_ALTER", alter_);
    toolbox::Save("OPTION_ROTATE", rotate_);
    toolbox::Save("OPTION_SOUND", sound_);
    toolbox::Save("GAME_OVER", game_over_);
    toolbox::Save("GAME_MOVES_COUNT", board_.moves_.size());
    for (std::size_t i = 0; i < board_.moves_.size(); ++i)
    {
        std::ostringstream composer;
        composer << "GAME_MOVES_" << i;
        toolbox::Save(composer.str().c_str(), (unsigned int)board_.moves_[i]);
    }
    toolbox::Save("GAME_LEVEL", (unsigned int)board_.level_);
    for (std::size_t i = 0; i < board_.fulls_.size(); ++i)
    {
        std::ostringstream composer;
        composer << "GAME_FULL_" << i;
        toolbox::Save(composer.str().c_str(), board_.fulls_.test(i));
    }
    for (std::size_t i = 0; i < board_.humans_.size(); ++i)
    {
        std::ostringstream composer;
        composer << "GAME_HUMAN_" << i;
        toolbox::Save(composer.str().c_str(), board_.humans_.test(i));
    }
    for (std::size_t i = 0; i < board_.queens_.size(); ++i)
    {
        std::ostringstream composer;
        composer << "GAME_QUEEN_" << i;
        toolbox::Save(composer.str().c_str(), board_.queens_.test(i));
    }
}

void main::Data::reset_all()
{
    difficulty_ = 4;
    alter_ = false;
    rotate_ = false;
    sound_ = false;
    reset_game();
}

void main::Data::reset_game()
{
    game_over_ = 0;
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
    for (auto& move : board_.moves_)
    {
        move = Board::cell_count_ - 1 - move;
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
