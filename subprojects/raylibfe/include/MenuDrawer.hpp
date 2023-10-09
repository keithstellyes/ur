#pragma once

#include <optional>

enum MenuOptionType {
    MOT_START_GAME_AI_ORACLE, MOT_START_GAME_AI_RANDOM, MOT_START_GAME_HOTSEAT
};

struct MenuOption {
    MenuOptionType menuOptionType: 3;
};

class MenuDrawer {
    // returns a MenuOption struct when something's been selected
    std::optional<MenuOption> step();
};