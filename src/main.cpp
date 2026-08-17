#include "main.h"

#include <istream>
#include <locale>
#include <ostream>

#include "data.h"
#include "menu.h"
#include "game.h"
#include "progress.h"

main::PROGRESS main::progress_ = main::PROGRESS::MENU;

void application::Restore(std::istream& input, Completion completion)
{
    std::locale::global(std::locale::classic());
    main::data_.load(input);
    completion();
}

void application::Checkpoint(std::ostream& output)
{
    main::data_.save(output);
}

std::unique_ptr<core::Stage> application::CreateStage()
{
    if (main::progress_ == main::PROGRESS::GAME)
    {
        return std::make_unique<main::Game>();
    }
    return std::make_unique<main::Menu>();
}
