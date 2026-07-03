/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Shared GUI team colors
*/

#pragma once

#include "Game/GameState.hpp"

#include <algorithm>
#include <array>
#include <string_view>

namespace GUI {

inline constexpr std::array<Color, 7> TeamColorPalette = {
    Colors::Blue,
    Colors::Red,
    Colors::Green,
    Colors::Orange,
    Colors::Purple,
    Colors::SkyBlue,
    Colors::Pink,
};

inline Color getTeamColor(const GameState &state, std::string_view teamName)
{
    auto team = std::find(state.teams.begin(), state.teams.end(), teamName);

    if (team == state.teams.end())
        return Colors::LightGray;
    std::size_t teamIndex = static_cast<std::size_t>(std::distance(state.teams.begin(), team));
    return TeamColorPalette[teamIndex % TeamColorPalette.size()];
}

}
