#include <bitset>
#include <condition_variable>
#include <cstring>
#include <list>
#include <mutex>
#include <sstream>

#include "helper.h"

#include "game.h"
#include "progress.h"

main::Game::Game()
    : stop_thinking_{false}
{
    handlers_["body"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "ready") == 0)
        {
            bridge::CallFunction("setup();");
        }
        else if (std::strcmp(command, "setup") == 0)
        {
            std::ostringstream js;
            js << "createBoard(" << Board::last_row_ << ","
               << Board::cell_count_ << ");";
            bridge::CallFunction(js.str().c_str());
            reset_board();
        }
    };
    handlers_["game"] = [&](const char *command, const char *info)
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
                play_audio("lose");
                join_threads();
            }
            data_.reset_game();
            reset_board();
        }
        else if (std::strcmp(command, "switch") == 0)
        {
            if (data_.game_over_ == 0)
            {
                play_audio("click");
                join_threads();
            }
            data_.switch_sides();
            reset_board();
        }
        else if (std::strcmp(command, "validate") == 0)
        {
            // Ignore a completion left behind by canceled work.
            if (notified_work_ != Work::Guess)
            {
                return;
            }
            if (guesser_.joinable())
            {
                guesser_.join();
            }
            notified_work_ = Work::None;
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
            // Ignore a completion left behind by canceled work.
            if (notified_work_ != Work::Think)
            {
                return;
            }
            if (thinker_.joinable())
            {
                thinker_.join();
            }
            notified_work_ = Work::None;
            data_.board_.moves_ = best_board_.moves_;
            if (data_.board_.moves_.empty())
            {
                draw();
            }
            else
            {
                data_.board_.traced_ = true;
                play_audio("click");
            }
            update_view();
        }
        else if (std::strcmp(command, "go") == 0)
        {
            if (data_.game_over_ == 0)
            {
                if (data_.board_.traced_ &&
                    !data_.board_.moves_.empty())
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
    handlers_["cell"] = [&](const char *command, const char *info)
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
                    play_audio("click");
                }
                else
                {
                    if (data_.board_.fulls_.test(index) &&
                        data_.board_.humans_.test(index))
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
                            play_audio("click");
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
                                play_audio("click");
                            }
                        }
                    }
                }
                update_view();
            }
        }
    };
}

main::Game::~Game()
{
    join_threads();
}

void main::Game::Attach()
{
    // A newly loaded view starts its worker from the setup handler.
    suspended_work_ = Work::None;
    notified_work_ = Work::None;
    bridge::SetAudioNoSolo(true);
    bridge::SetLayout(true, false);
    bridge::LoadView(Index(), "game");
}

void main::Game::Resume()
{
    bridge::SetScreenOn(true);
    const Work work = suspended_work_;
    suspended_work_ = Work::None;
    if (work == Work::Think)
    {
        think();
    }
    else if (work == Work::Guess)
    {
        guess();
    }
}

void main::Game::Suspend()
{
    bridge::SetScreenOn(false);
    suspended_work_ = Work::None;
    if (thinker_.joinable())
    {
        suspended_work_ = Work::Think;
    }
    else if (guesser_.joinable())
    {
        suspended_work_ = Work::Guess;
    }
    join_threads();
    // The runtime queues completions posted just before suspension.
    if (notified_work_ != Work::None)
    {
        suspended_work_ = Work::None;
    }
}

void main::Game::Escape()
{
    main::progress_ = PROGRESS::MENU;
    RequestStage();
}

void main::Game::play_audio(const char *audio)
{
    if (data_.sound_)
    {
        std::ostringstream js;
        js << "playAudio('" << audio << "');";
        bridge::CallFunction(js.str().c_str());
    }
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
    js.str("");
    js.clear();
    js << "setThumb(" << (data_.thumb_ ? "true" : "false") << ");";
    bridge::CallFunction(js.str().c_str());
}

void main::Game::validate_move()
{
    if (!guesser_.joinable())
    {
        data_.board_.traced_ = false;
        for (const auto &board : boards_)
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
    int message = 0, go = 0;
    std::bitset<Board::cell_count_> available_moves;
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

        if (data_.highlight_ && data_.board_.is_human() &&
            !guesser_.joinable())
        {
            const auto selected = data_.board_.moves_.size();
            for (const auto &board : boards_)
            {
                if (selected >= board.moves_.size())
                {
                    continue;
                }
                bool matches = true;
                for (std::size_t i = 0; i < selected; ++i)
                {
                    if (data_.board_.moves_[i] != board.moves_[i])
                    {
                        matches = false;
                        break;
                    }
                }
                if (matches)
                {
                    available_moves.set(board.moves_[selected]);
                }
            }
        }
    }

    std::ostringstream js;
    js << "renderBoard([";
    for (std::size_t i = 0; i < Board::cell_count_; ++i)
    {
        if (i != 0)
        {
            js << ",";
        }
        int piece = (!data_.board_.fulls_.test(i) ? 0 : data_.board_.humans_.test(i) ? -1
                                                                                     : 1) *
                    (data_.board_.queens_.test(i) ? 2 : 1);
        js << piece;
    }
    js << "],[";
    for (std::size_t i = 0; i < data_.board_.moves_.size(); ++i)
    {
        if (i != 0)
        {
            js << ",";
        }
        js << (unsigned int)data_.board_.moves_[i];
    }
    js << "],[";
    bool first_available = true;
    for (std::size_t i = 0; i < available_moves.size(); ++i)
    {
        if (available_moves.test(i))
        {
            if (!first_available)
            {
                js << ",";
            }
            js << i;
            first_available = false;
        }
    }
    js << "]," << data_.last_move_ << "," << data_.previous_move_ << ","
       << message << "," << go << ");";
    bridge::CallFunction(js.str().c_str());
}

void main::Game::move_human()
{
    data_.previous_move_ = data_.last_move_;
    data_.last_move_ = data_.board_.moves_.back();
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
        play_audio("move");
        think();
    }
}

void main::Game::move_cpu()
{
    data_.previous_move_ = data_.last_move_;
    data_.last_move_ = data_.board_.moves_.back();
    data_.board_.apply(best_board_);
    data_.board_.level_ = 0;
    data_.board_.moves_.clear();
    if (data_.board_.lost())
    {
        loose();
    }
    else
    {
        play_audio("move");
        guess();
    }
}

void main::Game::loose()
{
    data_.game_over_ = 2;
    game_over();
    play_audio("lose");
}

void main::Game::draw()
{
    data_.game_over_ = 3;
    game_over();
    play_audio("draw");
}

void main::Game::win()
{
    data_.game_over_ = 1;
    game_over();
    play_audio("win");
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
    if (guesser_.joinable())
    {
        guesser_.join();
    }
    if (thinker_.joinable())
    {
        thinker_.join();
    }
    stop_thinking_ = false;
}

void main::Game::think()
{
    data_.board_.traced_ = false;
    boards_.emplace_back(data_.board_);
    notified_work_ = Work::None;
    thinker_ = std::thread([this, index = Index()]()
                           {
        auto progress = boards_.begin();
        const auto concurrency = std::thread::hardware_concurrency();
        std::vector<std::thread> workers(concurrency == 0 ? 1 : concurrency);
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
                            progress != boards_.end();
                    });
                    if (stop_thinking_ || progress == boards_.end())
                    {
                        break;
                    }
                    std::list<Board>::iterator job = progress++;
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
                        if (job->level_ == data_.difficulty_ * 2)
                        {
                            for (auto it = options.begin();
                                it != options.end();)
                            {
                                it->evaluate();
                                if (it->score_ != 0.0f &&
                                    it->score_ != Board::win_score_ &&
                                    !it->has_legal_move())
                                {
                                    it->score_ = 1.0f;
                                }
                                job->apply_score(*it);
                                it = options.erase(it);
                            }
                            options.clear();
                        }
                    }
                    waker_lock.lock();
                    --worker_count;
                    --progress;
                    boards_.splice(boards_.end(), std::move(options));
                    ++progress;
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
            notified_work_ = Work::Think;
            bridge::AsyncMessage(index, "game", "play", "");
        }
        boards_.clear(); });
}

void main::Game::guess()
{
    data_.board_.traced_ = false;
    notified_work_ = Work::None;
    guesser_ = std::thread([this, index = Index()]()
                           {
        boards_ = data_.board_.list_options();
        if (!stop_thinking_)
        {
            notified_work_ = Work::Guess;
            bridge::AsyncMessage(index, "game", "validate", "");
        } });
}

void main::Game::FeedUri(const char *uri, std::function<void(
                                              const std::vector<unsigned char> &)> &&consume)
{
}
