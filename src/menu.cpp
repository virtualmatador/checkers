#include <sstream>
#include <cstring>

#include "helper.h"

#include "menu.h"
#include "data.h"
#include "progress.h"

main::Menu::Menu()
{
    handlers_["body"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "ready") == 0)
        {
            std::ostringstream js;
            js.str("");
            js.clear();
            js << "setDifficulty(" << data_.difficulty_ << ");";
            bridge::CallFunction(js.str().c_str());
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
            js << "setSound(" << (data_.sound_ ? "true" : "false") << ");";
            bridge::CallFunction(js.str().c_str());
            js.str("");
            js.clear();
            js << "setThumb(" << (data_.thumb_ ? "true" : "false") << ");";
            bridge::CallFunction(js.str().c_str());
            js.str("");
            js.clear();
            js << "setHighlight(" << (data_.highlight_ ? "true" : "false") << ");";
            bridge::CallFunction(js.str().c_str());
        }
    };
    handlers_["play"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
            Play();
    };
    handlers_["difficulty"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else
            {
                std::size_t difficulty = std::strtoul(info, nullptr, 10);
                if (difficulty > 0 &&
                    difficulty < Board::difficulty_limit_)
                {
                    data_.difficulty_ = difficulty;
                }
            }
        }
    };
    handlers_["alter"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else if (std::strcmp(info, "true") == 0)
            {
                data_.alter_ = true;
            }
            else if (std::strcmp(info, "false") == 0)
            {
                data_.alter_ = false;
            }
        }
    };
    handlers_["rotate"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else if (std::strcmp(info, "true") == 0)
            {
                data_.rotate_ = true;
            }
            else if (std::strcmp(info, "false") == 0)
            {
                data_.rotate_ = false;
            }
        }
    };
    handlers_["sound"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else if (std::strcmp(info, "true") == 0)
            {
                data_.sound_ = true;
            }
            else if (std::strcmp(info, "false") == 0)
            {
                data_.sound_ = false;
            }
        }
    };
    handlers_["thumb"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else if (std::strcmp(info, "true") == 0)
            {
                data_.thumb_ = true;
            }
            else if (std::strcmp(info, "false") == 0)
            {
                data_.thumb_ = false;
            }
        }
    };
    handlers_["highlight"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
            return;
        else if (std::strcmp(command, "click") == 0)
        {
            if (std::strlen(info) == 0)
            {
                return;
            }
            else if (std::strcmp(info, "true") == 0)
            {
                data_.highlight_ = true;
            }
            else if (std::strcmp(info, "false") == 0)
            {
                data_.highlight_ = false;
            }
        }
    };
    handlers_["reset"] = [&](const char *command, const char *info)
    {
        if (std::strlen(command) == 0)
        {
            return;
        }
        else if (std::strcmp(command, "click") == 0)
        {
            data_.reset_all();
            RequestStage();
        }
    };
}

main::Menu::~Menu()
{
}

void main::Menu::Attach()
{
    bridge::SetAudioNoSolo(true);
    bridge::SetLayout(false, false);
    bridge::LoadView(Index(), "menu");
}

void main::Menu::Escape()
{
    bridge::Exit();
}

void main::Menu::Play()
{
    progress_ = PROGRESS::GAME;
    RequestStage();
}

void main::Menu::FeedUri(const char *uri, std::function<void(
                                              const std::vector<unsigned char> &)> &&consume)
{
}
