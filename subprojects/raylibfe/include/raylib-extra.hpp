#pragma once

#include <cstddef>

#include "raylib.h"
#include "rlgl.h" 

// function removed from base library
// taken from https://github.com/raysan5/raylib/blob/master/examples/models/models_draw_cube_texture.c
void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

bool operator==(Vector3 const& a, Vector3 const& b);
bool operator==(BoundingBox const& a, BoundingBox const& b);

struct BBHashFunction
{
    std::size_t operator()(const BoundingBox& k) const
    {
        return k.min.x + (k.min.y * 10);
    }
};