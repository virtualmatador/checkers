#include "main.h"

#include "data.h"
#include "menu.h"
#include "game.h"
#include "progress.h"

main::PROGRESS main::progress_ = main::PROGRESS::MENU;

void life_cycle::Begin()
{
    main::data_.load();
}

void life_cycle::End()
{
    main::data_.save();
}

void life_cycle::Create()
{
}

void life_cycle::Destroy()
{
    main::data_.save();
}

void life_cycle::Start()
{
}

void life_cycle::Stop()
{
    main::data_.save();
}

void life_cycle::Restart()
{
    main::data_.save();
    switch (main::progress_)
    {
        case main::PROGRESS::MENU:
            core::Stage::stage_ = std::make_unique<main::Menu>();
        break;
        case main::PROGRESS::GAME:
            core::Stage::stage_ = std::make_unique<main::Game>();
        break;
    }
}

