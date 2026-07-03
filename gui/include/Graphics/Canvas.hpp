/*
** EPITECH PROJECT, 2026
** Project - Zappy
** File description:
** Raylib drawing adapter
*/

#pragma once

#include "Graphics/ICanvas.hpp"

namespace GUI {

class Canvas : public ICanvas {
    public:
        int width() const override;
        int height() const override;
        int scaleSize(int value) const override;
        int measureText(std::string_view text, int fontSize) const override;

        void drawFPS(int x, int y) const override;
        void drawText(std::string_view text, int x, int y, int fontSize, Color color) const override;
        void drawLine(int startX, int startY, int endX, int endY, Color color) const override;
        void drawRectangle(int x, int y, int width, int height, Color color) const override;
        void drawRectangleLines(int x, int y, int width, int height, Color color) const override;
        void drawCube(Vec3 pos, Vec3 size, Color color) const override;
        void drawCubeWires(Vec3 pos, Vec3 size, Color color) const override;
        void drawSphere(Vec3 pos, float radius, Color color) const override;
        void drawCylinder(Vec3 pos, float radius, float height, Color color) const override;
        void drawCircle3D(Vec3 center, float radius, Color color) const override;
        Color fade(Color color, float alpha) const override;
};

}
