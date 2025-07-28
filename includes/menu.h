#pragma once

#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <vector>
#include "raylib.h"
#include "config.h"
extern const Color BACKGROUND;
extern Config CONFIG;
class MatrixMenu;

class ListMenu {
    public:
        std::vector<bool> blockedFiles;
        std::vector<std::filesystem::path> arcs;
        int selected = 0, number = 10, first = 0;
        int off = number / 5;
        ListMenu(std::filesystem::path path);
        void Draw(void);
        MatrixMenu* tryOpenSelected(void);

};
class MatrixFrame {   
    public:
        std::vector<float> values;
        MatrixFrame(std::string line);
        void draw(Rectangle, Vector2, int, bool);
        
};
class MatrixMenu {
    public:
        std::vector<Vector2> dimensions;
        std::vector<MatrixFrame> frames;
        std::vector<bool> selectedFrames;
        const std::filesystem::path path;
        mutable Config config = Config();
                
        mutable int selectedDimension = 0, selectedFrame = 0, seletedPalette = 0, openedTab = 0;

        int nSize;
        MatrixMenu(const std::filesystem::path&);
        void draw(void);    
        void exportToEps() const;
        int selectedFrameCount() const;
};


