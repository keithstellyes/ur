#pragma once

#include <optional>

enum class InfoMenuDecision {
    OK
};

class InfoMenu {
public:
    std::optional<InfoMenuDecision> checkMenu();
};