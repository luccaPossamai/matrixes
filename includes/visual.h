#pragma once

#include "raylib.h"
#include <utility>

extern const Color BACKGROUND;
extern const Color BACKGROUND_LIGHT;
extern const Color BACKGROUND_BORDER;
extern const Color BACKGROUND_LIGHT_BORDER;
extern const Color FONT;
extern const Color BACKGROUND_SELECT_BORDER;
extern const Color BACKGROUND_SELECT;

extern void DrawRectangleWithBorder(Rectangle, float, std::pair<Color, Color>,
                                    std::pair<float, float>);

void DrawRoundedRectangleWithBorder(Rectangle rec, float thickness,
                                    float roundness, int segments,
                                    std::pair<Color, Color> colors,
                                    std::pair<float, float> overlay);
