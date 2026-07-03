/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** 3D map grid
*/


#include "Map.hpp"

#include "Game/TeamColors.hpp"

#include <algorithm>
#include <array>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// Y size of every resource model - center at half height above tile surface
static constexpr float RESOURCE_HEIGHT = 0.20F;

// Returns player_id -> stack slot index (0 = ground, 1 = above, ...).
// Players on the same tile are sorted by ID for a stable, deterministic order.
static std::unordered_map<int, int> buildStackIndex(const GUI::GameState &state)
{
    std::map<std::pair<int, int>, std::vector<int>> groups;
    for (const auto &[id, player] : state.players)
        groups[{player.x, player.y}].push_back(id);

    std::unordered_map<int, int> index;
    for (auto &[pos, ids] : groups) {
        std::sort(ids.begin(), ids.end());
        for (int i = 0; i < static_cast<int>(ids.size()); ++i)
            index[ids[i]] = i;
    }
    return index;
}

namespace GUI {

static void drawPlayerAura(const ICanvas &canvas, Vec3 pos, Color color)
{
    static constexpr int AURA_RINGS = 7;
    static constexpr float AURA_INNER_RADIUS = 0.72F;
    static constexpr float AURA_RADIUS_STEP = 0.05F;
    static constexpr float AURA_HEIGHT = 0.12F;
    Vec3 center = {pos.x, pos.y + AURA_HEIGHT, pos.z};

    canvas.drawCylinder(center, AURA_INNER_RADIUS + static_cast<float>(AURA_RINGS) * AURA_RADIUS_STEP,
        0.035F, canvas.fade(color, 0.55F));
    canvas.drawCircle3D(center, AURA_INNER_RADIUS - AURA_RADIUS_STEP, Colors::Black);
    for (int i = 0; i < AURA_RINGS; ++i)
        canvas.drawCircle3D(center, AURA_INNER_RADIUS + static_cast<float>(i) * AURA_RADIUS_STEP, color);
    canvas.drawCircle3D(center, AURA_INNER_RADIUS + static_cast<float>(AURA_RINGS) * AURA_RADIUS_STEP,
        Colors::Black);
}

Map::Map(ITheme &theme, float squareSize)
    : _theme(theme)
    , _squareSize(squareSize)
{
}

// Centers the grid on the world origin so the map is symmetrical around (0,0)
Vec3 Map::getTilePosition(float x, float y, const GameState &state, float height) const
{
    return {
        (x - static_cast<float>(state.mapWidth) / 2.0f) * _squareSize,
        height,
        (y - static_cast<float>(state.mapHeight) / 2.0f) * _squareSize,
    };
}

void Map::drawTiles(const ICanvas &canvas, const GameState &state) const
{
    for (std::size_t row = 0; row < state.mapHeight; ++row) {
        for (std::size_t col = 0; col < state.mapWidth; ++col) {
            // Y = -0.1f so the tile sits just below Y=0 where players stand
            Vec3 pos = getTilePosition(static_cast<int>(col), static_cast<int>(row), state, -0.1f);
            Vec3 size = { _squareSize - 0.1f, 0.2f, _squareSize - 0.1f };
            _theme.drawTile(canvas, pos, size, (col + row) % 2 == 0);
        }
    }
}

void Map::drawResources(const ICanvas &canvas, const GameState &state) const
{
    // Fraction of squareSize used to spread each resource slot from tile center
    static constexpr float resourceOffset = 0.28F;
    static constexpr std::array<Vec2, Zappy::Game::Resources::RESOURCE_COUNT> resourceSlots = {
        Vec2{-1.0F, -1.0F}, //? Food
        Vec2{0.0F, -1.0F},  //? Linemate
        Vec2{1.0F, -1.0F},  //? Deraumere
        Vec2{-1.0F, 0.0F},  //? Sibur
        Vec2{1.0F, 0.0F},   //? Mendiane
        Vec2{-1.0F, 1.0F},  //? Phiras
        Vec2{0.0F, 1.0F},   //? Thystame
    };

    for (const Tile &tile : state.tiles) {
        std::size_t resourceIndex = 0;
        for (unsigned int quantity : tile.resources) {
            if (quantity > 0) {
                // Y = half height so the model sits on the tile surface
                Vec3 pos = getTilePosition(tile.x, tile.y, state, RESOURCE_HEIGHT / 2.0F);
                pos.x += resourceSlots[resourceIndex].x * _squareSize * resourceOffset;
                pos.z += resourceSlots[resourceIndex].y * _squareSize * resourceOffset;
                _theme.drawResource(canvas, resourceIndex, pos);
            }
            ++resourceIndex;
        }
    }
}

void Map::drawPlayers(const ICanvas &canvas, const GameState &state, float currentTime) const
{
    auto stackIndex = buildStackIndex(state);
    float stackSpacing = _theme.getPlayerLabelHeight();
    for (const auto &[id, player] : state.players) {
        float height = static_cast<float>(stackIndex.at(id)) * stackSpacing;
        Player::DisplayPosition displayPos = player.getDisplayPosition(currentTime);
        Vec3 pos = getTilePosition(displayPos.x, displayPos.y, state, height);
        Player::AnimState animState = player.getEffectiveAnimState(currentTime);
        drawPlayerAura(canvas, pos, getTeamColor(state, player.teamName));
        _theme.drawPlayer(canvas, pos, player.rotationDeg, animState,
            player.getAnimationElapsed(currentTime, animState));
    }
}

void Map::drawEggs(const ICanvas &canvas, const GameState &state) const
{
    for (const auto &[id, egg] : state.eggs) {
        (void)id;
        Vec3 pos = getTilePosition(egg.x, egg.y, state, 0.35f);
        _theme.drawEgg(canvas, pos);
    }
}

void Map::draw(const ICanvas &canvas, const GameState &state, float currentTime) const
{
    drawTiles(canvas, state);
    drawResources(canvas, state);
    drawPlayers(canvas, state, currentTime);
    drawEggs(canvas, state);
}

void Map::drawCountLabel(const ICanvas &canvas, Vec2 screenPos, int fontSize, unsigned int quantity) const
{
    std::string label = "x" + std::to_string(quantity);
    int textWidth = canvas.measureText(label, fontSize);
    int sx = static_cast<int>(screenPos.x) - textWidth / 2;
    int sy = static_cast<int>(screenPos.y);
    canvas.drawText(label, sx - 1, sy, fontSize, Colors::Black); // outline for readability
    canvas.drawText(label, sx + 1, sy, fontSize, Colors::Black);
    canvas.drawText(label, sx, sy - 1, fontSize, Colors::Black);
    canvas.drawText(label, sx, sy + 1, fontSize, Colors::Black);
    canvas.drawText(label, sx, sy, fontSize, Colors::Yellow);
}

void Map::drawResourceLabels(const ICanvas &canvas, const GameState &state, const GameCamera &camera) const
{
    static constexpr float resourceOffset = 0.28F;
    static constexpr std::array<Vec2, Zappy::Game::Resources::RESOURCE_COUNT> resourceSlots = {
        Vec2{-1.0F, -1.0F}, //? Food
        Vec2{0.0F, -1.0F},  //? Linemate
        Vec2{1.0F, -1.0F},  //? Deraumere
        Vec2{-1.0F, 0.0F},  //? Sibur
        Vec2{1.0F, 0.0F},   //? Mendiane
        Vec2{-1.0F, 1.0F},  //? Phiras
        Vec2{0.0F, 1.0F},   //? Thystame
    };

    for (const Tile &tile : state.tiles) {
        std::size_t resourceIndex = 0;
        for (unsigned int quantity : tile.resources) {
            if (quantity > 1) {
                // Float label above the resource model
                Vec3 worldPos = getTilePosition(tile.x, tile.y, state, RESOURCE_HEIGHT + 0.3f);
                worldPos.x += resourceSlots[resourceIndex].x * _squareSize * resourceOffset;
                worldPos.z += resourceSlots[resourceIndex].y * _squareSize * resourceOffset;
                auto screenPos = camera.projectToScreen(worldPos);
                if (screenPos) {
                    float dist = camera.distanceTo(worldPos);
                    // Font scales inversely with distance, clamped to readable range
                    int fontSize = std::clamp(static_cast<int>(260.0f / dist), 18, 40);
                    drawCountLabel(canvas, *screenPos, fontSize, quantity);
                }
            }
            ++resourceIndex;
        }
    }
}

void Map::drawPlayerLabels(const ICanvas &canvas, const GameState &state, const GameCamera &camera,
    float currentTime) const
{
    auto stackIndex = buildStackIndex(state);
    float stackSpacing = _theme.getPlayerLabelHeight();
    for (const auto &[id, player] : state.players) {
        float height = static_cast<float>(stackIndex.at(id)) * stackSpacing + stackSpacing;
        Player::DisplayPosition displayPos = player.getDisplayPosition(currentTime);
        Vec3 labelPos = getTilePosition(displayPos.x, displayPos.y, state, height);

        auto screenPos = camera.projectToScreen(labelPos);
        if (!screenPos)
            continue;

        float dist = camera.distanceTo(labelPos);
        // Font scales inversely with distance, clamped to readable range
        int fontSize = std::clamp(static_cast<int>(_theme.getPlayerLabelScale() / dist), 8, 22);
        std::string label = player.teamName + " L" + std::to_string(player.level);
        int textWidth = canvas.measureText(label, fontSize);
        int sx = static_cast<int>(screenPos->x) - textWidth / 2;
        int sy = static_cast<int>(screenPos->y);

        canvas.drawText(label, sx + 1, sy + 1, fontSize, Colors::Black); // drop shadow
        canvas.drawText(label, sx, sy, fontSize, Colors::RayWhite);
    }
}

Vec3 Map::getPlayerWorldPos(const Player &player, const GameState &state, float currentTime) const
{
    auto stackIndex = buildStackIndex(state);
    float height = static_cast<float>(stackIndex.at(player.id)) * _theme.getPlayerLabelHeight();
    Player::DisplayPosition displayPos = player.getDisplayPosition(currentTime);
    return getTilePosition(displayPos.x, displayPos.y, state, height);
}

void Map::drawLabels(const ICanvas &canvas, const GameState &state, const GameCamera &camera, float currentTime) const
{
    drawResourceLabels(canvas, state, camera);
    drawPlayerLabels(canvas, state, camera, currentTime);
}

}
