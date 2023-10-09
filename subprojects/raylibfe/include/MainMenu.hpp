#pragma once

#include <optional>
#include <future>

enum class MainMenuDecision {
    StartGame, OpenInfo
};

class MainMenu {
public:
    std::optional<MainMenuDecision> checkMenu();
};
