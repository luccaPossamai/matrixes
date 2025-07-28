#include "config.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
static std::filesystem::path CONFIG = std::filesystem::current_path() / "config.json";

std::ifstream createConfigFile() {
    std::cerr << "Configuracao sendo carregada/criada..." << std::endl;
    if (std::filesystem::exists(CONFIG)) {
        return std::ifstream(CONFIG, std::ios::in);
    }

    // Cria e escreve arquivo padrão
    {
        std::ofstream out(CONFIG, std::ios::out | std::ios::trunc);
        if (!out.is_open()) {
            throw std::runtime_error("Erro ao criar arquivo de configuração");
        }

        nlohmann::json config;
        config["exports"]["eps"]["width"] = 200;
        config["palettes"]["example"]["0"] = "#0bd276";
        config["palettes"]["example"]["1"] = "#ff2537";
        config["palettes"]["example"]["-1"] = "#0b7dd2";
        config["palettes"]["example"]["2"] = "#f5fb54";
        config["palettes"]["example"]["-2"] = "#5e3363";

        out << config.dump(4);
        // Fecha automaticamente ao sair do escopo
    }

    // Agora abre para leitura
    std::ifstream in(CONFIG, std::ios::in);
    if (!in.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo de configuração para leitura");
    }
    return in;
}

Color ColorFromHex(const char *hex) {
    unsigned int r = 0, g = 0, b = 0;

    // Remove '#' se existir
    if (hex[0] == '#') hex++;

    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) != 3) {
        // Falha na leitura, retorna branco por padrão
        return WHITE;
    }

    Color color = {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};
    return color;
}

std::map<int, Color> readColorMapping(nlohmann::json config){
    std::map<int, Color> colors;
    try {
        for(auto& [key, value] : config.items()){
            if(value.is_string()){
                std::string hex = value.get<std::string>();
                colors[std::stoi(key)] = ColorFromHex(hex.c_str()); 
            }
            
        }
    } catch(std::exception){}
    return colors;
} 
  
Config::Config(){
    int res = 200;
    std::ifstream file = createConfigFile();
    nlohmann::json config;
    file >> config;
    try {res = config["exports"]["eps"]["width"];} catch(std::exception ex){}
    xResolution = res;
    for(auto& [key, value] : config["palettes"].items()){
        this->colors.push_back(readColorMapping(value));
    }

}


#include <cstdint>   // para uint8_t
#include <raylib.h>  // se estiver usando raylib
#include <cmath>     // para fmodf

Color GenerateColorFromIndex(int index) {
    // Mapeia o índice (incluindo negativos) para uint32_t de forma determinística
    
    uint32_t safeIndex = static_cast<uint32_t>(index + 100000); // desloca para valores positivos
    uint32_t hash = safeIndex * 2654435761u; // número primo grande (Knuth hash)
    //printf("\n%d %d %d\n", index, safeIndex, hash);
    // Normaliza para [0,1) usando fmod
    double r = std::fmod(hash * 0.001, 1.0f) * 255;
    double g = std::fmod(hash * 0.003, 1.0f) * 255;
    double b = std::fmod(hash * 0.007, 1.0f) * 255;
    unsigned char r1 = static_cast<unsigned char>(r),
        g1 = static_cast<unsigned char>(g),
        b1 = static_cast<unsigned char>(b);
    return {r1, g1, b1, 255};
}
Color Config::getColor(int mapIndex, int index){
    std::map<int, Color> map = mapIndex >= this->colors.size() ? this->colors.at(0) : this->colors.at(mapIndex);
    Color color;
    if(map.find(index) != map.end()){
        color = map[index];
    } else {
        color = GenerateColorFromIndex(index);
        
    }
    return color;
}

int Config::getWidth(){
    return this->xResolution;
}
