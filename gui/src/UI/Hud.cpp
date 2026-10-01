/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** GUI HUD component
*/

#include "UI/Hud.hpp"

#include "Game/ResourceColors.hpp"
#include "Game/TeamColors.hpp"

#include <algorithm>
#include <array>
#include <map>
#include <sstream>

static constexpr std::array<std::string_view, 4> hudPageNames = {
    "Overview",
    "Teams",
    "Players",
    "Resources",
};

namespace GUI {

void Hud::update(HudAction action)
{
    if (action == HudAction::ToggleVisibility) {
        _visible = !_visible;
        return;
    }
    if (!_visible)
        return;
    if (action == HudAction::NextPage) {
        _page = (_page + 1) % hudPageNames.size();
        _scroll = 0;
    }
    if (action == HudAction::PreviousPage) {
        _page = (_page + hudPageNames.size() - 1) % hudPageNames.size();
        _scroll = 0;
    }
    if (action == HudAction::ScrollDown)
        ++_scroll;
    if (action == HudAction::ScrollUp && _scroll > 0)
        --_scroll;
}

// Count only incantations that are currently running (status activated)
std::size_t Hud::getActiveIncantationCount(const GameState &state) const
{
    return std::count_if(state.incantations.begin(), state.incantations.end(),
        [](const Incantation &incantation) {
            return incantation.active;
        });
}

// Count players whose their team name matches the teamName we're currently inspecting
std::size_t Hud::getTeamPlayerCount(const GameState &state, std::string_view teamName) const
{
    return std::count_if(state.players.begin(), state.players.end(), [teamName](const auto &entry) {
        return entry.second.teamName == teamName;
    });
}

// Build the HUD level summary for one team, like "L1:2 L3:1" (L[playerLvl]:[nb])
std::string Hud::getTeamLevels(const GameState &state, std::string_view teamName) const
{
    // lvl / nbPlayers
    std::map<std::size_t, std::size_t> levels;
    std::ostringstream stream;

    for (const auto &[id, player] : state.players) {
        (void)id;
        if (player.teamName == teamName)
            ++levels[player.level];
    }
    if (levels.empty())
        return "-";
    for (const auto &[level, nbPlayers] : levels)
        stream << "L" << level << ":" << nbPlayers << " ";
    std::string result = stream.str();
    result.pop_back(); // delete the useless space char at the end of the string
    return result;
}

std::string Hud::getTeamLevelMinMax(const GameState &state, std::string_view teamName) const
{
    std::size_t minLevel = 0;
    std::size_t maxLevel = 0;
    bool found = false;

    for (const auto &[id, player] : state.players) {
        (void)id;
        if (player.teamName != teamName)
            continue;
        if (!found) {
            minLevel = player.level;
            maxLevel = player.level;
            found = true;
            continue;
        }
        minLevel = std::min(minLevel, player.level);
        maxLevel = std::max(maxLevel, player.level);
    }
    if (!found)
        return "-";
    return std::to_string(minLevel) + "-" + std::to_string(maxLevel);
}

void Hud::addPlayerFlagSegments(HudLine &line, const Player &player, float currentTime) const
{
    Player::AnimState state = player.getEffectiveAnimState(currentTime);
    if (state == Player::AnimState::Walk || state == Player::AnimState::Dead || state == Player::AnimState::LevelUp)
        return;
    line.push_back({"  ", Colors::RayWhite});
    if (state == Player::AnimState::Incantation)
        line.push_back({"I", Colors::Violet});
    if (state == Player::AnimState::LayingEgg)
        line.push_back({"E", Colors::Orange});
    if (state == Player::AnimState::Broadcast)
        line.push_back({"B", Colors::SkyBlue});
    if (state == Player::AnimState::Eject)
        line.push_back({"X", Colors::Red});
}

void Hud::addOverviewLines(const GameState &state, HudLines &lines) const
{
    lines.push_back({{"Map: ", Colors::SkyBlue}, {std::to_string(state.mapWidth) + "x" + std::to_string(state.mapHeight), Colors::RayWhite}});
    lines.push_back({{"Time: ", Colors::Orange}, {std::to_string(state.timeUnit), Colors::RayWhite}});
    lines.push_back({{"Players: ", Colors::Blue}, {std::to_string(state.players.size()), Colors::RayWhite}});
    lines.push_back({{"Eggs: ", Colors::LightGray}, {std::to_string(state.eggs.size()), Colors::RayWhite}});
    lines.push_back({{"Teams: ", Colors::Yellow}, {std::to_string(state.teams.size()), Colors::RayWhite}});
    lines.push_back({{"Incantations: ", Colors::Violet}, {std::to_string(getActiveIncantationCount(state)) + "/"
        + std::to_string(state.incantations.size()), Colors::RayWhite}});
    if (state.gameEnded)
        lines.push_back({{"Winner: ", Colors::Yellow}, {state.winningTeam, Colors::Yellow}});
    else
        lines.push_back({{"State: ", Colors::SkyBlue}, {"Running", Colors::Green}});
}

void Hud::addTeamsLines(const GameState &state, HudLines &lines) const
{
    for (const std::string &team : state.teams) {
        const std::size_t playerCount = getTeamPlayerCount(state, team);
        const Color teamColor = GUI::getTeamColor(state, team);

        lines.push_back({{team, teamColor}, {": " + std::to_string(playerCount) + " player"
            + (playerCount > 1 ? "s" : ""), Colors::RayWhite}});
        lines.push_back({{"  levels: ", Colors::RayWhite}, {getTeamLevels(state, team), Colors::RayWhite}});
        lines.push_back({{"  min/max: ", Colors::RayWhite}, {getTeamLevelMinMax(state, team), Colors::RayWhite}});
    }
    if (state.teams.empty())
        lines.push_back({{"No team", Colors::LightGray}});
}

void Hud::addPlayersForTeam(const GameState &state, HudLines &lines, std::string_view teamName,
    float currentTime) const
{
    bool found = false;
    const Color teamColor = GUI::getTeamColor(state, teamName);

    for (const auto &[id, player] : state.players) {
        if (player.teamName != teamName)
            continue;
        found = true;
        HudLine line = {
            {"  #" + std::to_string(id), teamColor},
            {" L" + std::to_string(player.level) + " (" + std::to_string(player.x) + ","
                + std::to_string(player.y) + ") " + std::to_string(static_cast<int>(player.rotationDeg))
                + "deg", Colors::RayWhite},
        };

        addPlayerFlagSegments(line, player, currentTime);
        lines.push_back(line);
    }
    if (!found)
        lines.push_back({{"  no player", Colors::LightGray}});
}

void Hud::addPlayersLines(const GameState &state, HudLines &lines, float currentTime) const
{
    for (const std::string &team : state.teams) {
        lines.push_back({{team, GUI::getTeamColor(state, team)}});
        addPlayersForTeam(state, lines, team, currentTime);
    }
    for (const auto &[id, player] : state.players) {
        if (std::find(state.teams.begin(), state.teams.end(), player.teamName) != state.teams.end())
            continue;
        const std::string teamName = player.teamName.empty() ? "No team" : player.teamName;
        HudLine line = {
            {teamName + " #" + std::to_string(id), Colors::LightGray},
            {" L" + std::to_string(player.level) + " (" + std::to_string(player.x) + ","
                + std::to_string(player.y) + ") " + std::to_string(static_cast<int>(player.rotationDeg))
                + "deg", Colors::RayWhite},
        };

        addPlayerFlagSegments(line, player, currentTime);
        lines.push_back(line);
    }
    if (state.players.empty())
        lines.push_back({{"No player", Colors::LightGray}});
}

void Hud::addResourceLine(HudLines &lines, std::string_view name, Color color, unsigned int mapQuantity,
    unsigned int playerQuantity) const
{
    lines.push_back({{std::string(name), color}});
    lines.push_back({{"  map=" + std::to_string(mapQuantity)
        + " players=" + std::to_string(playerQuantity)
        + " total=" + std::to_string(mapQuantity + playerQuantity), Colors::RayWhite}});
}

void Hud::addResourcesLines(const GameState &state, HudLines &lines) const
{
    Zappy::Game::Resources mapResources = {};
    Zappy::Game::Resources playerResources = {};

    for (const Tile &tile : state.tiles)
        mapResources += tile.resources;
    for (const auto &[id, player] : state.players) {
        (void)id;
        playerResources += player.inventory;
    }
    addResourceLine(lines, "Food", ResourceColors[0], mapResources.food, playerResources.food);
    addResourceLine(lines, "Linemate", ResourceColors[1], mapResources.linemate, playerResources.linemate);
    addResourceLine(lines, "Deraumere", ResourceColors[2], mapResources.deraumere, playerResources.deraumere);
    addResourceLine(lines, "Sibur", ResourceColors[3], mapResources.sibur, playerResources.sibur);
    addResourceLine(lines, "Mendiane", ResourceColors[4], mapResources.mendiane, playerResources.mendiane);
    addResourceLine(lines, "Phiras", ResourceColors[5], mapResources.phiras, playerResources.phiras);
    addResourceLine(lines, "Thystame", ResourceColors[6], mapResources.thystame, playerResources.thystame);
}

auto Hud::getLines(const GameState &state, float currentTime) const -> HudLines
{
    HudLines lines;

    if (_page == 0)
        addOverviewLines(state, lines);
    if (_page == 1)
        addTeamsLines(state, lines);
    if (_page == 2)
        addPlayersLines(state, lines, currentTime);
    if (_page == 3)
        addResourcesLines(state, lines);
    return lines;
}

void Hud::drawLine(const ICanvas &canvas, const HudLine &line, int x, int y, int fontSize) const
{
    int textX = x;

    for (const HudSegment &segment : line) {
        canvas.drawText(segment.text, textX, y, fontSize, segment.color);
        textX += canvas.measureText(segment.text, fontSize);
    }
}

void Hud::draw(const ICanvas &canvas, const GameState &state, float currentTime)
{
    if (!_visible)
        return;

    const int panelWidth = canvas.scaleSize(360);
    const int margin = canvas.scaleSize(16);
    const int padding = canvas.scaleSize(12);
    const int fontSize = canvas.scaleSize(22);
    const int lineHeight = canvas.scaleSize(27);
    const int x = canvas.width() - panelWidth - margin;
    const int y = margin;
    const int height = std::min(canvas.height() - margin * 2, canvas.scaleSize(650));
    const int titleY = y + padding;
    const int contentY = titleY + lineHeight + lineHeight / 2;
    const int footerY = y + height - padding - lineHeight;
    const int visibleLines = std::max(1, (footerY - contentY - lineHeight / 2) / lineHeight);
    HudLines lines = getLines(state, currentTime);
    int textY = contentY;
    const std::size_t maxScroll = lines.size() > static_cast<std::size_t>(visibleLines)
        ? lines.size() - static_cast<std::size_t>(visibleLines)
        : 0;

    if (_scroll > maxScroll)
        _scroll = maxScroll;

    const std::size_t end = std::min(lines.size(), _scroll + static_cast<std::size_t>(visibleLines));

    canvas.drawRectangle(x, y, panelWidth, height, canvas.fade(Colors::Black, 0.65F));
    canvas.drawRectangleLines(x, y, panelWidth, height, Colors::DarkGray);
    canvas.drawText(std::string("HUD ") + std::to_string(_page + 1) + "/" + std::to_string(hudPageNames.size())
        + " - " + std::string(hudPageNames[_page]), x + padding, titleY, fontSize, Colors::Yellow);

    for (std::size_t i = _scroll; i < end; ++i) {
        drawLine(canvas, lines[i], x + padding, textY, fontSize);
        textY += lineHeight;
    }

    canvas.drawText(std::string("<- page | scroll ")
        + std::to_string(_scroll) + "/" + std::to_string(maxScroll)
        + " | page ->", x + padding, footerY, fontSize, Colors::LightGray);
}

}
