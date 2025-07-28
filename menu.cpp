
#include "menu.h"
#include "config.h"
#include <cmath>
#include <cstdlib>
#include "raylib.h"
#include "raymath.h"
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <fstream>
#include <utility>

Config CONFIG = Config();
const  Color BACKGROUND = {15,15,15, 255};
const std::filesystem::path eps = std::filesystem::current_path() / "exports";

void DrawRectangleWithBorder(Rectangle rec, float thickness, std::pair<Color, Color> colors, std::pair<float, float> overlay){
    Rectangle border = {rec.x - thickness, rec.y - thickness, rec.width + 2 * thickness, rec.height + 2 * thickness};
    
    DrawRectangleRec(border, colors.second);
    DrawRectangleRec(rec, colors.first);
    if(overlay.second > 0) DrawRectangleLinesEx(rec, overlay.second, BLACK);
    if(overlay.first > 0) DrawRectangleLinesEx(border, overlay.first, BLACK);

}
void DrawRoundedRectangleWithBorder(Rectangle rec, float thickness, float roundness, int segments, std::pair<Color, Color> colors, std::pair<float, float> overlay){
    Rectangle border = {rec.x - thickness, rec.y - thickness, rec.width + 2 * thickness, rec.height + 2 * thickness};
    
    DrawRectangleRounded(border, roundness, segments, colors.second);
    DrawRectangleRounded(rec, roundness, segments, colors.first);
    if(overlay.second > 0) DrawRectangleRoundedLinesEx(border, roundness, segments, overlay.second, BLACK);
    if(overlay.first > 0) DrawRectangleRoundedLinesEx(rec, roundness, segments, overlay.first, BLACK);
}




ListMenu::ListMenu(std::filesystem::path path){
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file()) { 
                arcs.push_back(entry);
                blockedFiles.push_back(false);
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

void ListMenu::Draw(){
    int number = 10;
    Vector2 DIM = {(float)GetScreenWidth(), (float)GetScreenHeight()};
    int fontSize = 200 / number ;
    if(this->arcs.size() == 0){
        std::string t = "No matrixes to analyze"; 
        DrawText(t.c_str(), DIM.x/ 2.0f - 0.5 * MeasureText(t.c_str(), fontSize), DIM.y/ 2.0f, fontSize, DARKGRAY);
    } else {
        float f = 1.0 / number;
        float frac = 5;
        for(int i = first; i < first + number ; i++){
            Rectangle rectange = {(1 - frac) / 2.0f * DIM.x, (i - first) * f * DIM.y, frac * DIM.x, f * DIM.y};
            Color color = i == selected ? LIGHTGRAY : GRAY;
                        if(i >= this->arcs.size()){
                color = DARKGRAY;
            } else if(this->blockedFiles.at(i)){
                color = RED;

            }
            DrawRectangleWithBorder(rectange, 1, std::pair(color, DARKGRAY), std::pair(true, true));
            if(ColorIsEqual(color, DARKGRAY)) continue;
            std::string path = this->arcs.at(i).filename().c_str();
            DrawText(path.c_str(), (int) (DIM.x * 0.5f) - 0.5 * MeasureText(path.c_str(), fontSize), (int) (DIM.y * f * (i - first + 0.5f)) - fontSize / 2.0, fontSize, i == selected ? BLACK : DARKGRAY);
        }
        

           
    }

};


std::vector<std::string> splitBySpace(const std::string& str) {
    std::istringstream iss(str);
    std::vector<std::string> result;
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

MatrixMenu* ListMenu::tryOpenSelected(){
    MatrixMenu* menu = nullptr;
    try {
        menu = new MatrixMenu(this->arcs.at(this->selected));
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        this->blockedFiles.at(this->selected) = true;
    }
    return menu;
    
}
std::vector<Vector2> findPossibleDimensions(int N){
    std::vector<Vector2> dimensions;
    for(int i = 1; i * i <= N; i++){
        if(N % i == 0) dimensions.push_back({N / (float)i, (float)i});
    }
    return dimensions;
}





MatrixFrame::MatrixFrame(std::string line){
    std::vector<std::string> strings = splitBySpace(line);
    std::transform(strings.begin(), strings.end(), std::back_inserter(this->values), [](const std::string& s){
        return std::stof(s);
    });

}
void MatrixFrame::draw(Rectangle rectangle, Vector2 dim, int selectedColormap, bool f){
    int n = this->values.size();
    float size = f ?  rectangle.height / dim.y : rectangle.width / dim.x;
    for(int i = 0; i < n; i++){
        int x = i % (int)dim.x, y = i / (int)dim.x;
        Rectangle rec = {rectangle.x + (x * size), rectangle.y + (y * size), size, size};
        Color color = selectedColormap == 0 ? GenerateColorFromIndex(values.at(i)) :
            CONFIG.getColor(selectedColormap - 1, values.at(i));
        //color = this->values.at(i) > 0 ? RED : this->values.at(i) < 0 ? BLUE : BLACK;
        DrawRectangleRec(rec, color);
        //DrawRectangleLinesEx(rec, 1, BLACK);
    }
}
MatrixMenu::MatrixMenu(const std::filesystem::path& path) : path(path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::string errorS =  "Trying to open file " + std::string(path.relative_path()) + "\n";
        throw std::runtime_error(errorS);
    }

    std::string line;

    int n = -1, count  = 0;
    while (std::getline(file, line)) {
        MatrixFrame frame = MatrixFrame(line);
        this->frames.push_back(frame);
        this->selectedFrames.push_back(false);
        count++;
        if(n == -1){
            n = frame.values.size();
            if(n <= 0){
                std::string errorS =  "Could not find valid data for " + std::string(path.relative_path());
                throw std::runtime_error(errorS);
            }
        } else {
            if(n != frame.values.size()){
                std::string errorS =  "Found number of elements different on frame of index" + std::to_string(count)+"";
                throw std::runtime_error(errorS);
            }
        }
    
    }
    this->nSize = n;
    if(this->frames.empty()) throw std::runtime_error("Could not find valid data for " + std::string(path.relative_path()) + "\n");
    this->dimensions = findPossibleDimensions(this->frames.at(0).values.size());
    this->selectedDimension = this->dimensions.size() - 1;



}

void tryToExport(const MatrixMenu& menu, Rectangle frame){
    Vector2 dim = menu.dimensions.at(menu.selectedDimension);
    float thickness = frame.width / 250;
        float selThickness = 2 * thickness;
        int fontSize = 20;
        std::string s = "Do you want to export selected frames as eps files?";
        if(menu.selectedFrameCount() == 0) s = "Do you want to export current frame as eps file?";
        int textSize = MeasureText(s.c_str(), fontSize);
        Vector2 size = {textSize * 1.2f, textSize * 1.2f / 3.0f};
        Rectangle rec {
            frame.x + (frame.width - size.x) / 2.0f,
            frame.y + (frame.height - size.y) / 2.0f,
            size.x, size.y
        };
        DrawRectangleWithBorder(rec, thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
        DrawText(s.c_str(), rec.x + (rec.width - textSize)/2.0f, rec.y + rec.height / 3.0f - fontSize/2.0f, fontSize, WHITE);
        Rectangle yRec = {rec.x + rec.width / 4.0f -  size.x * 0.1f, rec.y + 2 * rec.height / 3.0f - fontSize/2.0f -  size.y * 0.1f, size.x * 0.2f, size.y * 0.2f};
        Rectangle nRec = {rec.x + 3 * rec.width / 4.0f -  size.x * 0.1f, rec.y + 2 * rec.height / 3.0f - fontSize/2.0f -  size.y * 0.1f, size.x * 0.2f, size.y * 0.2f};
        
        
        DrawRectangleWithBorder(yRec, thickness, std::pair(BACKGROUND, WHITE), std::pair(0,0));
        DrawRectangleWithBorder(nRec, thickness, std::pair(BACKGROUND, WHITE), std::pair(0,0));
        DrawText("n", nRec.x + (nRec.width - MeasureText("n", fontSize))/2.0f, nRec.y + nRec.height / 2 - fontSize/2.0f, fontSize, WHITE);
        DrawText("y", yRec.x + (yRec.width - MeasureText("y", fontSize))/2.0f, yRec.y + yRec.height / 2 - fontSize/2.0f, fontSize, WHITE);
        bool fY = IsKeyPressed(KEY_Y) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), yRec));
        bool fN = IsKeyPressed(KEY_N) || (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), nRec));
        if(fY) {
            menu.exportToEps();
        } 
        if(fY || fN){
            menu.openedTab = 0;
        } 
}
int MatrixMenu::selectedFrameCount() const{
    int s = 0;
    for(bool v : this->selectedFrames){
        s += v;
    }
    return s;
}
void drawPaletteColor(Rectangle frame, Rectangle paletteHeader,
        int palette, bool sel){
    float thickness = frame.width / 250;
    float selThickness = 2 * thickness;

    std::map<int, Color> map =  {
                                        {-1, GenerateColorFromIndex(-1)},
                                        {1, GenerateColorFromIndex(1)},
                                        {0, GenerateColorFromIndex(0)},
                                        {2, GenerateColorFromIndex(2)},
                                    };
        if(palette > 0) map = CONFIG.colors.at(palette - 1);

        int colorCount = std::min(4.f, (float)map.size()), count = 0;
        
        float colorSquareSize = paletteHeader.width / (3.50f),
                spaceSize = colorSquareSize / 2.0f;

 for(auto& pair : map){
            
            if(count >= colorCount) break;
            int x = count % 2, y = count / 2.0;
            Rectangle recColor = {paletteHeader.x + spaceSize + x * (spaceSize + colorSquareSize),
                                  paletteHeader.y + spaceSize + y * (spaceSize + colorSquareSize),
                                  (float)colorSquareSize, (float)colorSquareSize};
            DrawRectangleWithBorder(recColor, 1, std::pair(pair.second, WHITE), std::pair(0, 0));
            count++;

        }

}
void drawPalettesPopup(const MatrixMenu& menu, Rectangle frame){
    Vector2 dim = menu.dimensions.at(menu.selectedDimension);
    float thickness = frame.width / 250;
        float selThickness = 2 * thickness;

    
    int countMax = CONFIG.colors.size();
    float paletteIconSize = (float)1.1f * frame.width / 21.0f;
    
    Vector2 boxSize = {paletteIconSize * 21, 1.2f * paletteIconSize * (float)( 2.0f + 1.5f * floor(countMax / 10))};
    Rectangle box = {frame.x + (frame.width - boxSize.x) / 2.0f, frame.y, boxSize.x, boxSize.y};
    DrawRectangleWithBorder(box,thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    for(int i = 0; i < countMax; i++){
        int x = i % countMax, y = i / countMax;
        float dy = (frame.height - paletteIconSize) / 9.0f; 
        Rectangle palette = {
            box.x + paletteIconSize / 2.0f + x * (1.5f * paletteIconSize),
            box.y + 1.2f * paletteIconSize / 2.0f + y * (1.5f * paletteIconSize),
            paletteIconSize, 1.2f * paletteIconSize};
        
        bool sel = menu.seletedPalette == i;
        
        std::pair normal(BACKGROUND, WHITE);
        std::pair normalHeader(WHITE, WHITE);
        std::pair normalHeaderInverse(GREEN, GREEN);
        std::pair normalInverse(BACKGROUND, GREEN);
        Rectangle paletteHeader = {palette.x, std::ceil(palette.y), palette.width, 0.2f * palette.height - (sel ? selThickness : thickness)};

        DrawRectangleWithBorder(palette, sel ? selThickness : thickness, sel ? normalInverse : normal, std::pair(0, 0));
        DrawRectangleWithBorder(paletteHeader, sel ? selThickness : thickness, sel ? normalHeaderInverse : normalHeader, std::pair(0, 0));
        int textSize = MeasureText(std::to_string(i).c_str(), 4);
        int dySel = std::floor(sel ? -paletteHeader.height : -2 -paletteHeader.height / 2.0f) + 4;
        Color nColor = BLACK;
        DrawText(std::to_string(i).c_str(), paletteHeader.x + (paletteHeader.width - textSize)/ 2.0f, paletteHeader.y - dySel  - thickness, 4, nColor);
        DrawText(std::to_string(i).c_str(), 1 + paletteHeader.x + (paletteHeader.width - textSize)/ 2.0f, paletteHeader.y - dySel - thickness, 4, nColor);
        DrawText(std::to_string(i).c_str(), -1 + paletteHeader.x + (paletteHeader.width - textSize)/ 2.0f, paletteHeader.y - dySel - thickness, 4, nColor);
        

        if(i >= CONFIG.colors.size()) continue;
        drawPaletteColor(frame, {paletteHeader.x, paletteHeader.y + paletteHeader.height + (sel ? selThickness : thickness), paletteIconSize, paletteIconSize}, i, sel);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(CheckCollisionPointRec(GetMousePosition(), palette)) menu.seletedPalette = i;
        }
        if(IsKeyPressed(48 + i) || IsKeyPressed(320 + i)) menu.seletedPalette = i;
        
    } 
}

void drawDimensionsPopup(const MatrixMenu& menu, Rectangle frame){
     Vector2 dim = menu.dimensions.at(menu.dimensions.size() - 1);
    float thickness = frame.width / 250;
    float selThickness = 2 * thickness;

    
    int countMax = menu.dimensions.size();
    int n = 4;
    
    float paletteIconSizeX = (float)(1.1f * frame.width) / (1.5* n + 0.5);
    float paletteIconSizeY = paletteIconSizeX / 2.0f;
    
    
    Vector2 boxSize = {paletteIconSizeX * (1.5f* n + 0.5f), paletteIconSizeY * (float)( 2.0f + 1.5f * floor(countMax / n))};
    Rectangle box = {frame.x + (frame.width - boxSize.x) / 2.0f, frame.y, boxSize.x, boxSize.y};
    DrawRectangleWithBorder(box,thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    for(int i = 0; i < countMax; i++){
        int x = i % n, y = i / n;
        bool sel = menu.selectedDimension == i;
        Rectangle rec = {box.x + paletteIconSizeX / 2.0f + x *(1.5f * paletteIconSizeX),
                            box.y + paletteIconSizeY / 2.0f + y * (1.5f * paletteIconSizeY),
                            paletteIconSizeX , paletteIconSizeY};
        DrawRectangleWithBorder(rec, sel ? selThickness : thickness, sel ? std::pair(BACKGROUND, GREEN) : std::pair(BACKGROUND, WHITE), std::pair(0, 0));
        std::string s = std::to_string((int)menu.dimensions.at(i).x) + " x " + std::to_string((int)menu.dimensions.at(i).y);
        int fontSize = paletteIconSizeY / 2.5f;
        DrawText(s.c_str(), rec.x + (rec.width - MeasureText(s.c_str(), fontSize)) / 2.0f, rec.y + (rec.height - fontSize) / 2.0f, fontSize, WHITE);
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            if(CheckCollisionPointRec(GetMousePosition(), rec)) menu.selectedDimension = i;
        }
        if(IsKeyPressed(48 + i) || IsKeyPressed(320 + i)) menu.selectedDimension = i;
    }
}
void MatrixMenu::draw(void){
    int w = GetScreenWidth(), h = GetScreenHeight();
    Vector2 dim = this->dimensions.at(this->selectedDimension);
    Vector2 baseDim = {(float)sqrt(nSize), (float)sqrt(nSize)};
    float frac = 0.85;
    bool f = frac*w*dim.y/dim.x > frac * h;
    Rectangle frame = {}, baseFrame = {};
    if(f) {
        frame.width = frac*h*dim.x/dim.y; frame.height = frac*h;
        baseFrame.width = frac*h*baseDim.x/baseDim.y; baseFrame.height = frac*h;
    } else {
        frame.width = frac*w; frame.height = frac*w*dim.y/dim.x;
        baseFrame.width = frac*w; baseFrame.height = frac*w*baseDim.y/baseDim.x;
    }
    frame.x = (w - frame.width) / 2.0f; frame.y = (h - frame.height) / 2.0f;
    baseFrame.x = (w - baseFrame.width) / 2.0f; baseFrame.y = (h - baseFrame.height) / 2.0f;
    
    float thickness = baseFrame.width / 250;
    float selThickness = 2 * thickness;
   
    
    Rectangle bg = {(float)thickness, (float)thickness, w - 2.f * thickness, (float)h - 2 * thickness};
    DrawRoundedRectangleWithBorder(bg, thickness, 0.05, 12, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    
    bool sel = this->selectedFrames.at(selectedFrame);
    DrawRectangleWithBorder(frame, sel ? selThickness : thickness, sel ? std::pair(BACKGROUND, GREEN) : std::pair(BACKGROUND, WHITE), std::pair(false, false));
    this->frames.at(this->selectedFrame).draw(frame, dim, this->seletedPalette, f);
    int restBottomHeight = (h - baseFrame.height) / 2.0f,
          loadingBarHeight = 0.6f * restBottomHeight,
          loadingBarY = baseFrame.y + baseFrame.height + restBottomHeight / 2.0f - loadingBarHeight / 2.0f;
    Rectangle loadingBar = {
        baseFrame.x, (float)loadingBarY, baseFrame.width, (float)loadingBarHeight
    };
    DrawRectangleWithBorder(loadingBar, thickness, std::pair(DARKGRAY, WHITE), std::pair(0, 0));
    int frameBarWidthSize = baseFrame.width / this->frames.size();
    int frameBarWidth = std::max(0.5f*loadingBarHeight, (float)frameBarWidthSize);
    
    
    for(int index = 0; index < this->frames.size(); index++){
        if(!selectedFrames.at(index)) continue;
        int dx1 = this->frames.size() == 1 ? 0 : index * ( baseFrame.width - frameBarWidth) / (this->frames.size() - 1);

        Rectangle frameBar1 = {baseFrame.x + dx1, (float)loadingBarY, (float)frameBarWidth, (float)loadingBarHeight};
        
        DrawRectangleWithBorder(frameBar1,thickness, std::pair(DARKGREEN, BLANK), std::pair(0, 0));

    }
    float dx = this->frames.size() == 1 ? 0 : selectedFrame * ( baseFrame.width - frameBarWidth) / (this->frames.size() -1);
    Rectangle frameBar = {baseFrame.x + dx, (float)loadingBarY, (float)frameBarWidth, (float)loadingBarHeight};
    
    DrawRectangleWithBorder(frameBar,thickness,std::pair(selectedFrames.at(selectedFrame) ? GREEN : GRAY, selectedFrames.at(selectedFrame) ? WHITE : WHITE), std::pair(0, 0));

        std::string s = std::to_string(this->selectedFrame);
    float fontSize = loadingBarHeight / 2.0f;

    Rectangle icon = {baseFrame.x, (h - baseFrame.height) / 4.0f - loadingBarHeight / 2.0f , (float)loadingBarHeight, (float)loadingBarHeight};
    float squareSize =loadingBarHeight / 3.5f;
    DrawRectangleWithBorder(icon, thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    drawPaletteColor(baseFrame, icon, this->seletedPalette, false);
    Rectangle icon2 = {baseFrame.x + 1.5F * loadingBarHeight, (h - baseFrame.height) / 4.0f - loadingBarHeight / 2.0f , (float)loadingBarHeight, (float)loadingBarHeight};

    DrawRectangleWithBorder(icon2, thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    std::string prop = std::to_string((int)this->dimensions.at(this->selectedDimension).x) + " x " + std::to_string((int)this->dimensions.at(this->selectedDimension).y);
    int fontSizeProp = loadingBarHeight / 2.5f;
    int propW = MeasureText(prop.c_str(), fontSizeProp);

    Rectangle icon3 = {baseFrame.x + baseFrame.width, (h - baseFrame.height) / 4.0f - loadingBarHeight / 2.0f , (float)1.2f * propW, (float)loadingBarHeight};
    icon3.x -= icon3.width;
    DrawRectangleWithBorder(icon3, thickness, std::pair(BACKGROUND, WHITE), std::pair(0, 0));
    DrawText(prop.c_str(), icon3.x + (icon3.width - propW) / 2.0f, icon3.y + (icon3.height - fontSizeProp) / 2.0f, fontSizeProp, WHITE);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(GetMousePosition(), icon)) openedTab = openedTab == 1 ? 0 : 1;
        if(CheckCollisionPointRec(GetMousePosition(), icon2)) openedTab = openedTab == 2 ? 0 : 2;
        if(CheckCollisionPointRec(GetMousePosition(), icon3)) openedTab = openedTab == 3 ? 0 : 3;
    }
    //printf("\n%d %d %d\n", openedTab, GetKeyPressed(), MOUSE_LEFT_BUTTON);
    if(this->openedTab == 1) drawPalettesPopup(*this, baseFrame);
    if(this->openedTab == 2) tryToExport(*this, baseFrame);
    if(this->openedTab == 3) drawDimensionsPopup(*this, baseFrame);
}

void MatrixMenu::exportToEps() const{

    
    //epsPath = eps / epsPath;
    std::filesystem::path dirPath = this->path.parent_path() / this->path.stem();

    try {
        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directory(dirPath);
            std::cout << "Diretório criado em: " << dirPath << "\n";
        } else {
            std::cout << "Diretório já existe em: " << dirPath << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro ao criar diretório: " << e.what() << "\n";
    }
    std::vector<int> ints;
    if(selectedFrameCount() == 0) {
        ints.push_back(this->selectedFrame);
    } else {
        int count = 0;
        for(const auto& val : selectedFrames) {
            if(val) {
                ints.push_back(count);
            }
            count++;
        }  
    }
    for(int frameIndex : ints){
        std::filesystem::path epsPath = this->path.filename();
        epsPath.replace_filename(epsPath.stem().string() + "_frame_" + std::to_string(frameIndex));
        epsPath.replace_extension(".eps");
        epsPath = dirPath / epsPath;
        std::ofstream out(epsPath);

    if(std::filesystem::exists(epsPath)){
        std::cerr << "The eps file already exists at path " << epsPath.relative_path() << "\n";
    } 

    if (!out.is_open()) {
        std::cerr << "Failed creating eps file at " << epsPath.relative_path() << "\n";
        return;
    }
   

    out << std::fixed << std::setprecision(3);
    MatrixFrame frame = this->frames.at(frameIndex);
    Vector2 res = this->dimensions.at(selectedDimension);
    float f = res.x / res.y;

    int sizeX = CONFIG.getWidth();
    out << "%!PS-Adobe-3.0 EPSF-3.0\n";
    out << "%%BoundingBox: 0 0 " << sizeX << " " << (int)(sizeX / f) << "\n";
    int unitSize = (float)sizeX / res.x;
    if(unitSize <= 1){
        std::cerr << "Failed creating eps file, dimension bigger than pixel size.";
        return;

    }
    for(int i = 0; i < frame.values.size(); i++){
        int x = i % (int)res.x, y = res.y - 1- (int)(i / res.x);
        Color color = GenerateColorFromIndex(frame.values.at(i));
        if(this->seletedPalette > 0) CONFIG.getColor(this->seletedPalette - 1, frame.values.at(i));
        out << color.r / 255.0f << " " << color.g / 255.0f<< " " << color.b / 255.0f<< " setrgbcolor\n";
        out << "newpath\n";
        out << x * unitSize << " " << y * unitSize<< " moveto\n";
        out << unitSize << " 0 rlineto\n";
        out << "0 " << unitSize << " rlineto\n";
        out << -unitSize << " 0 rlineto\n";
        out << "closepath fill\n";
        
    }
    out << "showpage\n";
    }
    
}
