/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Raylib drawing adapter
*/

#include "Graphics/Canvas.hpp"
#include "UI/Scale.hpp"

#include "raylib.h"

#include <string>

static ::Color toRaylibColor(GUI::Color color)
{
    return {color.r, color.g, color.b, color.a};
}

static ::Vector3 toRaylibVec3(GUI::Vec3 value)
{
    return {value.x, value.y, value.z};
}

namespace GUI {

int Canvas::width() const
{
    return GetScreenWidth();
}

int Canvas::height() const
{
    return GetScreenHeight();
}

int Canvas::scaleSize(int value) const
{
    return UI::scaleSize(value, height());
}

// Reused across calls so passing a std::string_view to raylib's null-terminated
// C API doesn't allocate a fresh buffer on every draw call.
// WARN: not safe to call twice in the same expression — each call overwrites the same buffer.
static const char *toCString(std::string_view text)
{
    static thread_local std::string buffer;
    buffer.assign(text);
    return buffer.c_str();
}

int Canvas::measureText(std::string_view text, int fontSize) const
{
    return MeasureText(toCString(text), fontSize);
}

void Canvas::drawFPS(int x, int y) const
{
    DrawFPS(x, y);
}

void Canvas::drawText(std::string_view text, int x, int y, int fontSize, Color color) const
{
    DrawText(toCString(text), x, y, fontSize, toRaylibColor(color));
}

void Canvas::drawLine(int startX, int startY, int endX, int endY, Color color) const
{
    DrawLine(startX, startY, endX, endY, toRaylibColor(color));
}

void Canvas::drawRectangle(int x, int y, int width, int height, Color color) const
{
    DrawRectangle(x, y, width, height, toRaylibColor(color));
}

void Canvas::drawRectangleLines(int x, int y, int width, int height, Color color) const
{
    DrawRectangleLines(x, y, width, height, toRaylibColor(color));
}

void Canvas::drawCube(Vec3 pos, Vec3 size, Color color) const
{
    DrawCubeV(toRaylibVec3(pos), toRaylibVec3(size), toRaylibColor(color));
}

void Canvas::drawCubeWires(Vec3 pos, Vec3 size, Color color) const
{
    DrawCubeWiresV(toRaylibVec3(pos), toRaylibVec3(size), toRaylibColor(color));
}

void Canvas::drawSphere(Vec3 pos, float radius, Color color) const
{
    DrawSphere(toRaylibVec3(pos), radius, toRaylibColor(color));
}

void Canvas::drawCylinder(Vec3 pos, float radius, float height, Color color) const
{
    DrawCylinder(toRaylibVec3(pos), radius, radius, height, 48, toRaylibColor(color));
}

void Canvas::drawCircle3D(Vec3 center, float radius, Color color) const
{
    DrawCircle3D(toRaylibVec3(center), radius, {1.0F, 0.0F, 0.0F}, 90.0F, toRaylibColor(color));
}

Color Canvas::fade(Color color, float alpha) const
{
    ::Color faded = Fade(toRaylibColor(color), alpha);
    return {faded.r, faded.g, faded.b, faded.a};
}

}
