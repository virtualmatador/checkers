#include "main.h"

#include <locale>

#include "data.h"
#include "menu.h"
#include "game.h"
#include "progress.h"

main::PROGRESS main::progress_ = main::PROGRESS::MENU;

void application::Restore(Completion completion)
{
    std::locale::global(std::locale::classic());
    main::data_.load();
    completion();
}

void application::Checkpoint()
{
    main::data_.save();
}

std::unique_ptr<core::Stage> application::CreateStage()
{
    if (main::progress_ == main::PROGRESS::GAME)
    {
        return std::make_unique<main::Game>();
    }
    return std::make_unique<main::Menu>();
}
