/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** GUI HUD component
*/

#pragma once

#include "Game/GameState.hpp"
#include "Graphics/ICanvas.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace GUI {

enum class HudAction {
    ToggleVisibility,
    NextPage,
    PreviousPage,
    ScrollUp,
    ScrollDown,
};

class Hud {
    public:
        Hud() = default;
        ~Hud() = default;

        void update(HudAction action);
        void draw(const ICanvas &canvas, const GameState &state, float currentTime);

    private:
        struct HudSegment {
            std::string text;
            Color color;
        };

        using HudLine = std::vector<HudSegment>;
        using HudLines = std::vector<HudLine>;

        std::size_t getActiveIncantationCount(const GameState &state) const;
        std::size_t getTeamPlayerCount(const GameState &state, std::string_view teamName) const;
        std::string getTeamLevels(const GameState &state, std::string_view teamName) const;
        std::string getTeamLevelMinMax(const GameState &state, std::string_view teamName) const;
        void addPlayerFlagSegments(HudLine &line, const Player &player, float currentTime) const;
        void addOverviewLines(const GameState &state, HudLines &lines) const;
        void addTeamsLines(const GameState &state, HudLines &lines) const;
        void addPlayersForTeam(const GameState &state, HudLines &lines, std::string_view teamName,
            float currentTime) const;
        void addPlayersLines(const GameState &state, HudLines &lines, float currentTime) const;
        void addResourceLine(HudLines &lines, std::string_view name, Color color, unsigned int mapQuantity,
            unsigned int playerQuantity) const;
        void addResourcesLines(const GameState &state, HudLines &lines) const;
        HudLines getLines(const GameState &state, float currentTime) const;
        void drawLine(const ICanvas &canvas, const HudLine &line, int x, int y, int fontSize) const;

        bool _visible = true;
        std::size_t _page = 0;
        std::size_t _scroll = 0;
};

}
