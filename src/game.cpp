#include <cstring>
#include <limits>
#include <list>
#include <sstream>

#include "helper.h"

#include "game.h"
#include "progress.h"

main::Game::Game()
    : stop_thinking_{ false }
{
    handlers_["body"] = [&](const char* command, const char* info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "ready") == 0)
        {
            update_view();
            game_over();
            if (data_.board_.level_ != 0)
            {
                think();
            }
        }
    };
    handlers_["game"] = [&](const char* command, const char* info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "stop") == 0)
        {
            Escape();
        }
        else if (std::strcmp(command, "reset") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (data_.sound_)
                {
                    //bridge::PlayAudio(31);
                }
            }
            data_.reset_game();
            update_view();
        }
        else if (std::strcmp(command, "giveup") == 0)
        {
            if (data_.game_over_ == 0)
            {
                give_up();
            }
        }
        else if (std::strcmp(command, "play") == 0)
        {
            std::istringstream cells{ info };
            int cell;
            while (cells >> cell)
            {
                data_.moves_.emplace_back(cell);
            }
            update_view();
        }
    };
    handlers_["cell"] = [&](const char* command, const char* info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (data_.game_over_ == 0)
            {
                int index = std::strtol(info, nullptr, 10);
                if (data_.board_.level_ == 0)
                {
                    if (data_.moves_.empty())
                    {
                        if (data_.board_.is_full(index) &&
                            data_.board_.is_human(index))
                        {
                            data_.moves_.emplace_back(index);
                        }
                    }
                    else
                    {
                        if (data_.moves_[0] == index)
                        {
                            if (data_.moves_.size() > 1)
                            {
                                bool valid_move = false;
                                std::list<Board> boards;
                                std::list<std::vector<int>> moves;
                                data_.board_.list_options(boards, moves);
                                auto board = boards.begin();
                                auto move = moves.begin();
                                while (move != moves.end())
                                {
                                    valid_move =
                                        move->size() == data_.moves_.size();
                                    if (valid_move)
                                    {
                                        for (std::size_t i = 0;
                                            i < move->size(); ++i)
                                        {
                                            if ((*move)[i] != data_.moves_[i])
                                            {
                                                valid_move = false;
                                                break;
                                            }
                                        }
                                    }
                                    if (valid_move)
                                    {
                                        break;
                                    }
                                    ++board;
                                    ++move;
                                }
                                if (valid_move)
                                {
                                    data_.board_.fulls_ = board->fulls_;
                                    data_.board_.humans_ = board->humans_;
                                    data_.board_.level_ = 1;
                                    data_.moves_.clear();
                                    if (data_.board_.won())
                                    {
                                        data_.game_over_ = 1;
                                        game_over();
                                        if (data_.sound_)
                                        {
                                            //bridge::PlayAudio(30);
                                        }
                                    }
                                    else
                                    {
                                        think();
                                        if (data_.sound_)
                                        {
                                            //bridge::PlayAudio(29);
                                        }
                                    }
                                }
                                else
                                {
                                    if (data_.sound_)
                                    {
                                        //bridge::PlayAudio(28);
                                    }
                                }
                            }
                            else
                            {
                                data_.moves_.clear();
                            }
                        }
                        else
                        {
                            if (data_.board_.is_full(index))
                            {
                                if (data_.board_.is_human(index))
                                {
                                    data_.moves_.clear();
                                    data_.moves_.emplace_back(index);
                                }
                            }
                            else
                            {
                                data_.moves_.emplace_back(index);
                            }
                        }
                    }
                    update_view();
                }
                else if (!data_.moves_.empty() && index == data_.moves_[0])
                {
                    data_.board_.fulls_ = best_board_.fulls_;
                    data_.board_.humans_ = best_board_.humans_;
                    data_.board_.level_ = 0;
                    data_.moves_.clear();
                    if (data_.board_.lost())
                    {
                        give_up();
                    }
                    update_view();
                }
                else
                {
                    if (data_.sound_)
                    {
                        //bridge::PlayAudio(error);
                    }
                }
            }
        }
    };
    bridge::LoadWebView(index_,
        (std::int32_t)core::VIEW_INFO::AudioNoSolo, "game", "");
}

main::Game::~Game()
{
    stop_thinking_ = true;
    if (thinker_.joinable())
    {
        thinker_.join();
    }
}

void main::Game::Escape()
{
    main::progress_ = PROGRESS::MENU;
    bridge::NeedRestart();
}

void main::Game::update_view()
{
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        std::ostringstream js;
        js << "setSide(" << i << "," <<
            (data_.board_.is_full(i) ? data_.board_.is_human(i) ? 1 : 2 : 0) <<
            ");";
        bridge::CallFunction(js.str().c_str());
    }
    for (std::size_t i = 0; i < data_.moves_.size(); ++i)
    {
        std::ostringstream js;
        js << "setOrder(" << data_.moves_[i] << "," << i << ");";
        bridge::CallFunction(js.str().c_str());
    }
}

void main::Game::give_up()
{
    data_.game_over_ = 2;
    game_over();
    if (data_.sound_)
    {
        //bridge::PlayAudio(31);
    }
}

void main::Game::game_over()
{
    std::ostringstream js;
    js.str("");
    js.clear();
    js << "gameOver(" << data_.game_over_ << ");";
    bridge::CallFunction(js.str().c_str());
}

void main::Game::think()
{
    thinker_ = std::thread([this]()
    {
        std::list<Board> boards;
        std::list<std::vector<int>> moves;
        data_.board_.list_options(boards, moves);
        for (auto& board : boards)
        {
            if (stop_thinking_)
            {
                break;
            }
            if (board.level_ < 10)
            {
                board.score_ = board.level_ % 2 == 1 ?
                    std::numeric_limits<short>::max() :
                    std::numeric_limits<short>::min();
                board.list_options(boards);
            }
            else
            {
                board.evaluate();
            }
        }
        for (auto board = boards.crbegin(); board != boards.crend(); ++board)
        {
            if (stop_thinking_)
            {
                break;
            }
            if (board->level_ % 2 == 1)
            {
                board->parent_->score_ =
                    std::max(board->parent_->score_, board->score_);
            }
            else
            {
                board->parent_->score_ =
                    std::min(board->parent_->score_, board->score_);                    
            }
        }
        short best_score = std::numeric_limits<short>::min();
        std::vector<int>* best_move;
        auto board = boards.begin();
        auto move = moves.begin();
        while (!stop_thinking_ && move != moves.end())
        {
            if (best_score < board->score_)
            {
                best_score = board->score_;
                best_move = &(*move);
            }
            ++board;
            ++move;
        }
        if (!stop_thinking_)
        {
            best_board_ = *board;
            std::ostringstream info;
            for (const auto& cell : *best_move)
            {
                info << cell << ' ';
            }
            bridge::PostThreadMessage(
                index_, "game", "play", info.str().c_str());
        }
    });
}
