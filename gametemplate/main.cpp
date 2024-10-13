#include "GameTemplate.h"

#include "modules/game/jleKickStarter.h"

int
main(int argc, char *argv[])
{
    auto kickstarter = jleKickStarter{};
    kickstarter.kickStart<GameTemplate>(argc, argv);
    return 0;
}