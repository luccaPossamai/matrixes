
#include <cassert>
#include <unistd.h>
#include <filesystem> 
#include "raylib.h"
#include "raymath.h"

#include "menu.h"

#define RATE 200;

#ifndef COMPILE_DIR
#define COMPILE_DIR "undefined"
#endif

const std::filesystem::path COMPILE_PATH = COMPILE_DIR;

std::filesystem::path matrixes(){
    return COMPILE_PATH / "matrixes";

}


static ListMenu* menu = nullptr;
static MatrixMenu* matrixMenu = nullptr;
void render();
void tryOpenPath(int, char**);
void readKeys();

int main(int argc, char* argv[]){
    if(COMPILE_PATH == "undefined"){
        std::cerr << "Projected compiled without project path defined";
        return 1;
    }
    tryOpenPath(argc, argv);  
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(700, 700, "Matrixes");
    if(std::filesystem::exists(matrixes()) && !matrixMenu)menu = new ListMenu(matrixes());
    float acc = 100;
    while(!WindowShouldClose()){
        
        if(acc >= 1.0 / 60.0f){
            readKeys();
            acc = 0;
            render();
            
        }
        acc += GetFrameTime();
       
    }
     
}
void tryOpenPath(int argc, char* argv[]){
    if( argc < 2 ){
        std::cerr << "Uso: " << argv[0] << " <arquivo>\n";
        return;
    }
    std::filesystem::path caminho_arquivo = argv[1];
    caminho_arquivo = std::filesystem::absolute(caminho_arquivo);

    if (!std::filesystem::exists(caminho_arquivo)) {
        std::cerr << "File not founded at: " << caminho_arquivo << "\n";
        return;
    }

    std::cout << "Opening: " << caminho_arquivo << "\n";
    //std::cout << "Nome do arquivo: " << caminho_arquivo.filename() << "\n";
    //std::cout << "Extensão: " << caminho_arquivo.extension() << "\n";
    //std::cout << "Diretório pai: " << caminho_arquivo.parent_path() << "\n";
    SetTraceLogLevel(LOG_NONE);
    matrixMenu = new MatrixMenu(caminho_arquivo);
}
void render(){
    BeginDrawing();
        ClearBackground(BACKGROUND);
        if(matrixMenu){
            matrixMenu->draw();
        } else if(menu) {
            menu->Draw();
        }
        EndDrawing();



}

void readKeys(){
    float scroll = GetMouseWheelMove();

    int add = IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT) ? +1 
            : IsKeyPressed(KEY_UP) ||IsKeyPressed(KEY_LEFT)     ? -1 
            : 0;
    add += IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressedRepeat(KEY_RIGHT) ? +1 
            : IsKeyPressedRepeat(KEY_UP) ||IsKeyPressedRepeat(KEY_LEFT)     ? -1 
            : 0;

    add += scroll > 0 ? -1 : scroll < 0 ? +1 : 0;
        if(matrixMenu){
            matrixMenu->selectedFrame = Clamp(matrixMenu->selectedFrame + add, 0, matrixMenu->frames.size() - 1);
            if(IsKeyPressed(KEY_SPACE) || IsKeyPressedRepeat(KEY_SPACE)){
                matrixMenu->selectedFrames.at(matrixMenu->selectedFrame) = !matrixMenu->selectedFrames.at(matrixMenu->selectedFrame);
                matrixMenu->selectedFrame = Clamp(matrixMenu->selectedFrame + 1, 0, matrixMenu->frames.size() - 1);
            }
        } else if(menu){
            menu->selected = Clamp(menu->selected + add, 0, menu->arcs.size() - 1);
            int firstAdd = menu->selected - menu->first < menu->off && add < 0 ? -1 :
                            menu->first + menu->number - 1- menu->selected < menu->off && add > 0 ? +1 :0 ; 
            menu->first = Clamp(menu->first + firstAdd, 0, menu->arcs.size() - menu->number);
        if(IsKeyPressed(KEY_ENTER)){
            matrixMenu = menu->tryOpenSelected();
        }
    }
}

