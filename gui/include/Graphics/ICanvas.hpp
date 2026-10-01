/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Drawing surface abstraction (backend-agnostic)
*/

#pragma once

#include "Graphics/Types.hpp"
#include <string_view>

namespace GUI {

class ICanvas {
    public:
        virtual ~ICanvas() = default;

        virtual int width() const = 0;
        virtual int height() const = 0;
        virtual int scaleSize(int value) const = 0;
        virtual int measureText(std::string_view text, int fontSize) const = 0;

        virtual void drawFPS(int x, int y) const = 0;
        virtual void drawText(std::string_view text, int x, int y, int fontSize, Color color) const = 0;
        virtual void drawLine(int startX, int startY, int endX, int endY, Color color) const = 0;
        virtual void drawRectangle(int x, int y, int width, int height, Color color) const = 0;
        virtual void drawRectangleLines(int x, int y, int width, int height, Color color) const = 0;
        virtual void drawCube(Vec3 pos, Vec3 size, Color color) const = 0;
        virtual void drawCubeWires(Vec3 pos, Vec3 size, Color color) const = 0;
        virtual void drawSphere(Vec3 pos, float radius, Color color) const = 0;
        virtual void drawCylinder(Vec3 pos, float radius, float height, Color color) const = 0;
        virtual void drawCircle3D(Vec3 center, float radius, Color color) const = 0;
        virtual Color fade(Color color, float alpha) const = 0;
};

}
