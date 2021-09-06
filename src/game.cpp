#include <condition_variable>
#include <cstring>
#include <limits>
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
                    //bridge::PlayAudio(31);
                }
                join_threads();
            }
            data_.reset_game();
            reset_board();
        }
        else if (std::strcmp(command, "giveup") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (data_.sound_)
                {
                    //bridge::PlayAudio(31);
                }
                join_threads();
                give_up();
                update_view();
            }
        }
        else if (std::strcmp(command, "draw") == 0)
        {
            guesser_.join();
            draw();
            update_view();
        }
        else if (std::strcmp(command, "play") == 0)
        {
            thinker_.join();
            std::istringstream cells{ info };
            int cell;
            while (cells >> cell)
            {
                data_.moves_.emplace_back(cell);
            }
            if (data_.moves_.empty())
            {
                draw();
            }
            update_view();
        }
        else if (std::strcmp(command, "go") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (!data_.moves_.empty())
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
            if (data_.game_over_ == 0)
            {
                if (data_.board_.level_ == 0)
                {
                    int index = std::strtol(info, nullptr, 10);
                    if (!data_.moves_.empty() &&
                        data_.moves_.back() == index)
                    {
                        data_.moves_.pop_back();
                    }
                    else
                    {
                        if (data_.board_.fulls_.test(index))
                        {
                            if (data_.board_.humans_.test(index))
                            {
                                if (!data_.moves_.empty() &&
                                    data_.moves_[0] != index)
                                {
                                    data_.moves_.clear();
                                }
                                data_.moves_.emplace_back(index);
                            }
                        }
                        else
                        {
                            if (!data_.moves_.empty())
                            {
                                data_.moves_.emplace_back(index);
                            }
                        }
                    }
                    update_view();
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
    if (guesser_.joinable())
    {
        guesser_.join();
    }
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
            data_.moves_.clear();
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
    for (std::size_t i = 0; i < data_.moves_.size(); ++i)
    {
        std::ostringstream js;
        js << "setOrder(" << data_.moves_[i] << "," << i << ");";
        bridge::CallFunction(js.str().c_str());
    }
    int message = 0, go = 0;
    if (data_.game_over_ == 0)
    {
        if (data_.moves_.size() > 1)
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
    Board const * request = nullptr;
    for (const auto& board : boards_)
    {
        auto move = board.trace_moves();
        bool valid_move = move.size() == data_.moves_.size();
        if (valid_move)
        {
            for (std::size_t i = 0;
                i < move.size(); ++i)
            {
                if (move[i] != data_.moves_[i])
                {
                    valid_move = false;
                    break;
                }
            }
        }
        if (valid_move)
        {
            request = &board;
            break;
        }
    }
    if (request)
    {
        data_.board_.apply(*request);
        boards_.clear();
        data_.board_.level_ = 1;
        data_.moves_.clear();
        if (data_.board_.won())
        {
            win();
        }
        else
        {
            if (data_.sound_)
            {
                //bridge::PlayAudio(29);
            }
            think();
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

void main::Game::move_cpu()
{
    if (!data_.moves_.empty())
    {
        data_.board_.apply(best_board_);
        data_.board_.level_ = 0;
        data_.moves_.clear();
        if (data_.board_.lost())
        {
            give_up();
        }
        else
        {
            if (data_.sound_)
            {
                //bridge::PlayAudio();
            }
            guess();
        }
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

void main::Game::draw()
{
    data_.game_over_ = 3;
    game_over();
    if (data_.sound_)
    {
        //bridge::PlayAudio(32);
    }
}

void main::Game::win()
{
    data_.game_over_ = 1;
    game_over();
    if (data_.sound_)
    {
        //bridge::PlayAudio(30);
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
    thinker_ = std::thread([this, index = index_]()
    {
        boards_.emplace_back(data_.board_);
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
                    std::list<Board> options;
                    bool human = job->is_human();
                    for (std::size_t i = 0; i < Board::cell_count_; ++i)
                    {
                        if (job->fulls_.test(i))
                        {
                            if (human == job->humans_.test(i))
                            {
                                job->list_options(options, i, human);
                            }
                        }
                    }
                    if (options.empty())
                    {
                        job->evaluate();
                    }
                    else
                    {
                        job->score_ = human ?
                            std::numeric_limits<float>::max() :
                            -std::numeric_limits<float>::max();
                        if (job->level_ == 6)
                        {
                            for (auto& option : options)
                            {
                                option.evaluate();
                                job->apply_score(option);
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
            auto parent = board->parent_;
            while (parent->level_ == board->level_)
            {
                parent = parent->parent_;
            }
            if (board->level_ > 2)
            {
                parent->apply_score(*board);
            }
            else
            {
                auto score = parent->score_;
                auto score_level_ = parent->score_level_;
                parent->apply_score(*board);
                if (score != parent->score_ ||
                    score_level_ != parent->score_level_)
                {
                    parent->parent_ = &(*board);
                }
            }
        }
        std::ostringstream info;
        if (!stop_thinking_)
        {
            if (boards_.front().parent_)
            {
                best_board_ = *boards_.front().parent_;
                auto moves = best_board_.trace_moves();
                for (const auto& cell : moves)
                {
                    info << cell << ' ';
                }
            }
            bridge::PostThreadMessage(index, "game", "play", info.str().c_str());
        }
        boards_.clear();
    });
}

void main::Game::guess()
{
    guesser_ = std::thread([this, index = index_]()
    {
        for (std::size_t i = 0; i < Board::cell_count_; ++i)
        {
            if (stop_thinking_)
            {
                break;
            }
            if (data_.board_.fulls_.test(i))
            {
                if (data_.board_.humans_.test(i))
                {
                    data_.board_.list_options(boards_, i, true);
                }
            }
        }
        if (!stop_thinking_)
        {
            if (boards_.empty())
            {
                bridge::PostThreadMessage(index, "game", "draw", "");
            }
        }
    });
}
