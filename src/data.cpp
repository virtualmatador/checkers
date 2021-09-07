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
            0UL, Board::difficulty_limit_);
        toolbox::Load("OPTION_ALTER", alter_, false, false);
        toolbox::Load("OPTION_ROTATE", rotate_, false, false);
        toolbox::Load("OPTION_SOUND", sound_, false, false);
        toolbox::Load("GAME_OVER", game_over_, 0, 4);
        int moves_count;
        toolbox::Load("GAME_MOVES_COUNT", moves_count,
            0, (int)Board::cell_count_);
        for (std::size_t i = 0; i < moves_count; ++i)
        {
            std::ostringstream composer;
            int buffer;
            composer << "GAME_MOVES_" << i;
            toolbox::Load(composer.str().c_str(), buffer,
                -1, (int)Board::cell_count_);
            moves_.emplace_back(buffer);
        }
        toolbox::Load("GAME_LEVEL", board_.level_,
            (unsigned short)0, (unsigned short)2);
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
    toolbox::Save("GAME_MOVES_COUNT", moves_.size());
    for (std::size_t i = 0; i < moves_.size(); ++i)
    {
        std::ostringstream composer;
        composer << "GAME_MOVES_" << i;
        toolbox::Save(composer.str().c_str(), moves_[i]);
    }
    toolbox::Save("GAME_LEVEL", board_.level_);
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
    difficulty_ = 5;
    alter_ = false;
    rotate_ = false;
    sound_ = false;
    reset_game();
}

void main::Data::reset_game()
{
    game_over_ = false;
    moves_.clear();
    board_.level_ = 0;
    board_.score_ = 0;
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        data_.board_.fulls_.set(i, i < Board::piece_count_ ||
            i > Board::cell_count_ - 1 - Board::piece_count_);
        data_.board_.humans_.set(i,
            i > Board::cell_count_ - 1 - Board::piece_count_);
        data_.board_.queens_.set(i, false);
    }
}
