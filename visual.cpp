#include "visual.h"

const Color BACKGROUND = {15, 17, 21, 255};
const Color BACKGROUND_LIGHT = {28, 32, 38, 255};
const Color BACKGROUND_BORDER = {42, 47, 58, 255};
const Color BACKGROUND_LIGHT_BORDER = {52, 60, 72, 255};
const Color BACKGROUND_SELECT_BORDER = {64, 140, 255, 255};
const Color BACKGROUND_SELECT = {45, 105, 200, 255};
const Color FONT = {230, 232, 235, 255};

void DrawRectangleWithBorder(Rectangle rec, float thickness,
                             std::pair<Color, Color> colors,
                             std::pair<float, float> overlay) {
  Rectangle border = {rec.x - thickness, rec.y - thickness,
                      rec.width + 2 * thickness, rec.height + 2 * thickness};

  DrawRectangleRec(border, colors.second);
  DrawRectangleRec(rec, colors.first);
  if (overlay.second > 0)
    DrawRectangleLinesEx(rec, overlay.second, BLACK);
  if (overlay.first > 0)
    DrawRectangleLinesEx(border, overlay.first, BLACK);
}

void DrawRoundedRectangleWithBorder(Rectangle rec, float thickness,
                                    float roundness, int segments,
                                    std::pair<Color, Color> colors,
                                    std::pair<float, float> overlay) {
  Rectangle border = {rec.x - thickness, rec.y - thickness,
                      rec.width + 2 * thickness, rec.height + 2 * thickness};

  DrawRectangleRounded(border, roundness, segments, colors.second);
  DrawRectangleRounded(rec, roundness, segments, colors.first);
  if (overlay.second > 0)
    DrawRectangleRoundedLinesEx(border, roundness, segments, overlay.second,
                                BLACK);
  if (overlay.first > 0)
    DrawRectangleRoundedLinesEx(rec, roundness, segments, overlay.first, BLACK);
}
