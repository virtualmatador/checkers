#include <condition_variable>
#include <cstring>
#include <list>
#include <mutex>
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
            for (std::size_t i = 0; i < Board::cell_count_; ++i)
            {
                std::ostringstream js;
                js << "createCell(" << i << "," << Board::get_column(i) <<
                    "," << Board::get_row(i) <<
                    "," << Board::get_row(i) % 2 << ");";
                bridge::CallFunction(js.str().c_str());
            }
            reset_board();
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
                    bridge::PlayAudio(1);
                }
                join_threads();
            }
            data_.reset_game();
            reset_board();
        }
        else if (std::strcmp(command, "switch") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (data_.sound_)
                {
                    bridge::PlayAudio(3);
                }
                join_threads();
            }
            data_.switch_sides();
            reset_board();
        }
        else if (std::strcmp(command, "validate") == 0)
        {
            guesser_.join();
            if (!boards_.empty())
            {
                validate_move();
            }
            else
            {
                draw();
            }
            update_view();
        }
        else if (std::strcmp(command, "play") == 0)
        {
            thinker_.join();
            data_.board_.moves_ = best_board_.moves_;
            if (data_.board_.moves_.empty())
            {
                draw();
            }
            else
            {
                data_.board_.traced_ = true;
                if (data_.sound_)
                {
                    bridge::PlayAudio(3);
                }
            }
            update_view();
        }
        else if (std::strcmp(command, "go") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (!data_.board_.moves_.empty())
                {
                    if (data_.board_.level_ == 0)
                    {
                        if (guesser_.joinable())
                        {
                            guesser_.join();
                        }
                        move_human();
                    }
                    else
                    {
                        move_cpu();
                    }
                    update_view();
                }
            }
        }
    };
    handlers_["cell"] = [&](const char* command, const char* info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (data_.game_over_ == 0 && data_.board_.level_ == 0)
            {
                int index = std::strtol(info, nullptr, 10);
                if (!data_.board_.moves_.empty() &&
                    data_.board_.moves_.back() == index)
                {
                    data_.board_.moves_.pop_back();
                    validate_move();
                    if (data_.sound_)
                    {
                        bridge::PlayAudio(3);
                    }
                }
                else
                {
                    if (data_.board_.fulls_.test(index))
                    {
                        if (data_.board_.humans_.test(index))
                        {
                            if (!data_.board_.moves_.empty() &&
                                data_.board_.moves_[0] != index)
                            {
                                data_.board_.moves_.clear();
                            }
                            if (data_.board_.moves_.size() < Board::max_moves_)
                            {
                                data_.board_.moves_.emplace_back(index);
                                validate_move();
                                if (data_.sound_)
                                {
                                    bridge::PlayAudio(3);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!data_.board_.moves_.empty())
                        {
                            if (data_.board_.moves_.size() < Board::max_moves_)
                            {
                                data_.board_.moves_.emplace_back(index);
                                validate_move();
                                if (data_.sound_)
                                {
                                    bridge::PlayAudio(3);
                                }
                            }
                        }
                    }
                }
                update_view();
            }
        }
    };
    bridge::LoadWebView(index_, (std::int32_t)core::VIEW_INFO::AudioNoSolo |
        (std::int32_t)core::VIEW_INFO::Portrait, "game", "e0 e1 e2 p0 p1");
}

main::Game::~Game()
{
    join_threads();
}

void main::Game::Escape()
{
    main::progress_ = PROGRESS::MENU;
    bridge::NeedRestart();
}

void main::Game::reset_board()
{
    boards_.clear();
    if (data_.game_over_ == 0)
    {
        if (data_.board_.level_ != 0)
        {
            data_.board_.moves_.clear();
            think();
        }
        else
        {
            guess();
        }
    }
    set_preferences();
    update_view();
    game_over();
}

void main::Game::set_preferences()
{
    std::ostringstream js;
    js.str("");
    js.clear();
    js << "setAlter(" << (data_.alter_ ? "true" : "false") << ");";
    bridge::CallFunction(js.str().c_str());
    js.str("");
    js.clear();
    js << "setRotate(" << (data_.rotate_ ? "true" : "false") << ");";
    bridge::CallFunction(js.str().c_str());
}

void main::Game::validate_move()
{
    if (!guesser_.joinable())
    {
        data_.board_.traced_ = false;
        for (const auto& board : boards_)
        {
            bool valid_move = board.moves_.size() == data_.board_.moves_.size();
            if (valid_move)
            {
                for (std::size_t i = 0;
                    i < board.moves_.size(); ++i)
                {
                    if (board.moves_[i] != data_.board_.moves_[i])
                    {
                        valid_move = false;
                        break;
                    }
                }
            }
            if (valid_move)
            {
                best_board_ = board;
                data_.board_.traced_ = true;
                break;
            }
        }
    }
}

void main::Game::update_view()
{
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        int piece = (!data_.board_.fulls_.test(i) ? 0 :
            data_.board_.humans_.test(i) ? -1 : 1) *
            (data_.board_.queens_.test(i) ? 2 : 1);
        std::ostringstream js;
        js << "setPiece(" << i << "," << piece << ");";
        bridge::CallFunction(js.str().c_str());
    }
    for (std::size_t i = 0; i < data_.board_.moves_.size(); ++i)
    {
        std::ostringstream js;
        js << "setOrder(" << (unsigned int)data_.board_.moves_[i] <<
            "," << i << ");";
        bridge::CallFunction(js.str().c_str());
    }
    int message = 0, go = 0;
    if (data_.game_over_ == 0)
    {
        if (data_.board_.traced_)
        {
            go = data_.board_.is_human() ? 1 : 2;
        }
        else
        {
            message = data_.board_.is_human() ? 1 : 2;
        }
    }
    {
        std::ostringstream js;
        js << "setMessage(" << message << ");";
        bridge::CallFunction(js.str().c_str());
    }
    {
        std::ostringstream js;
        js << "setGo(" << go << ");";
        bridge::CallFunction(js.str().c_str());
    }
}

void main::Game::move_human()
{
    data_.board_.apply(best_board_);
    boards_.clear();
    data_.board_.level_ = 1;
    data_.board_.moves_.clear();
    if (data_.board_.won())
    {
        win();
    }
    else
    {
        if (data_.sound_)
        {
            bridge::PlayAudio(4);
        }
        think();
    }
}

void main::Game::move_cpu()
{
    data_.board_.apply(best_board_);
    data_.board_.level_ = 0;
    data_.board_.moves_.clear();
    if (data_.board_.lost())
    {
        loose();
    }
    else
    {
        if (data_.sound_)
        {
            bridge::PlayAudio(4);
        }
        guess();
    }
}

void main::Game::loose()
{
    data_.game_over_ = 2;
    game_over();
    if (data_.sound_)
    {
        bridge::PlayAudio(1);
    }
}

void main::Game::draw()
{
    data_.game_over_ = 3;
    game_over();
    if (data_.sound_)
    {
        bridge::PlayAudio(2);
    }
}

void main::Game::win()
{
    data_.game_over_ = 1;
    game_over();
    if (data_.sound_)
    {
        bridge::PlayAudio(0);
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

void main::Game::join_threads()
{
    stop_thinking_ = true;
    if (data_.board_.is_human())
    {
        if (guesser_.joinable())
        {
            guesser_.join();
        }
    }
    else
    {
        if (thinker_.joinable())
        {
            thinker_.join();
        }
    }
    stop_thinking_ = false;
}

void main::Game::think()
{
    data_.board_.traced_ = false;
    boards_.emplace_back(data_.board_);
    thinker_ = std::thread([this, index = index_]()
    {
        auto progress = std::prev(boards_.begin());
        std::vector<std::thread> workers{ std::thread::hardware_concurrency() };
        std::size_t worker_count = 0;
        std::mutex boards_lock;
        std::condition_variable waker;
        for (auto& worker : workers)
        {
            worker = std::thread([&]()
            {
                for (;;)
                {
                    std::unique_lock<std::mutex> waker_lock{ boards_lock };
                    waker.wait(waker_lock, [&]()
                    {
                        return stop_thinking_ ||
                            worker_count == 0 ||
                            std::next(progress) != boards_.end();
                    });
                    if (stop_thinking_ || std::next(progress) == boards_.end())
                    {
                        break;
                    }
                    std::list<Board>::iterator job = ++progress;
                    ++worker_count;
                    waker_lock.unlock();
                    std::list<Board> options = job->list_options();
                    if (options.empty())
                    {
                        job->evaluate();
                        if (job->score_ != 0 &&
                            job->score_ != Board::win_score_)
                        {
                            job->score_ = 1.0;
                        }
                    }
                    else
                    {
                        job->score_ = job->is_human() ?
                            Board::win_score_ + 1.0f :
                            -1.0f;
                        if (job->level_ == data_.difficulty_)
                        {
                            for (auto it = options.begin();
                                it != options.end();)
                            {
                                it->evaluate();
                                job->apply_score(*it);
                                it = options.erase(it);
                            }
                            options.clear();
                        }
                    }
                    waker_lock.lock();
                    --worker_count;
                    boards_.splice(boards_.end(), std::move(options));
                    waker_lock.unlock();
                    waker.notify_all();
                }
            });
        }
        for (auto& worker : workers)
        {
            worker.join();
        }
        workers.clear();
        for (auto board = boards_.rbegin(); board != boards_.rend(); ++board)
        {
            if (stop_thinking_)
            {
                break;
            }
            if (board->level_ == 1)
            {
                break;
            }
            if (board->level_ > 2)
            {
                board->parent_->apply_score(*board);
            }
            else
            {
                auto score = board->parent_->score_;
                auto score_level_ = board->parent_->score_level_;
                board->parent_->apply_score(*board);
                if (score != board->parent_->score_ ||
                    score_level_ != board->parent_->score_level_)
                {
                    board->parent_->parent_ = &(*board);
                }
            }
        }
        std::ostringstream info;
        if (!stop_thinking_)
        {
            if (boards_.front().parent_)
            {
                best_board_ = *boards_.front().parent_;
            }
            else
            {
                best_board_.moves_.clear();
            }
            bridge::PostThreadMessage(index, "game", "play", "");
        }
        boards_.clear();
    });
}

void main::Game::guess()
{
    data_.board_.traced_ = false;
    guesser_ = std::thread([this, index = index_]()
    {
        boards_ = data_.board_.list_options();
        if (!stop_thinking_)
        {
            bridge::PostThreadMessage(index, "game", "validate", "");
        }
    });
}
