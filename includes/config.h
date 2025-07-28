#pragma once
#include "json.hpp"
#include "raylib.h"
#include <map>
#include <vector>

extern Color GenerateColorFromIndex(int);


class Config{
    private:
        int xResolution;
    public:
         std::vector<std::map<int, Color>> colors;
        Config();
        Color getColor(int, int);
        int getWidth();
};
