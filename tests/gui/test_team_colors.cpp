/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Unit tests for GUI team colors
*/

#include <criterion/criterion.h>

#include "Game/TeamColors.hpp"

static void assert_color_eq(GUI::Color actual, GUI::Color expected)
{
    cr_assert_eq(actual.r, expected.r);
    cr_assert_eq(actual.g, expected.g);
    cr_assert_eq(actual.b, expected.b);
    cr_assert_eq(actual.a, expected.a);
}

Test(team_colors, first_team_is_blue)
{
    GUI::GameState state;

    state.teams = {"alpha"};
    assert_color_eq(GUI::getTeamColor(state, "alpha"), GUI::Colors::Blue);
}

Test(team_colors, second_team_is_red)
{
    GUI::GameState state;

    state.teams = {"alpha", "beta"};
    assert_color_eq(GUI::getTeamColor(state, "beta"), GUI::Colors::Red);
}

Test(team_colors, palette_wraps)
{
    GUI::GameState state;

    state.teams = {"a", "b", "c", "d", "e", "f", "g", "h"};
    assert_color_eq(GUI::getTeamColor(state, "h"), GUI::Colors::Blue);
}

Test(team_colors, missing_team_is_light_gray)
{
    GUI::GameState state;

    state.teams = {"alpha"};
    assert_color_eq(GUI::getTeamColor(state, "missing"), GUI::Colors::LightGray);
}
