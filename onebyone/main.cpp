#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <random>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <SDL2/SDL.h>
std::vector<std::string> csv_handler(const std::string &s){
    std::vector<std::string> result;
    std::string buffer= "";
    bool quoted = false;
    for(char c: s){
        if(quoted){
            if(c == '"'){
                quoted = false;
            }
            else{
                buffer += c;
            }
            continue;
        }

        if(c == ','){
            result.push_back(buffer);
            buffer = "";
        }
        else if(c == '"'){
            quoted = true;
        }
        else{
            buffer += c;
        }
    }
    result.push_back(buffer);
    return result;
}

int main(int argc, char** argv){
    
    /* Rules about Rules
       - The CSV must be in the order you want reveals to be in
       - The line format is name, response, Mu, Sigma
       - DRPs and Dummies are not supported at this time.
       TODO: Integrate this all with a vote parser 
     */
    std::vector<std::string> names;
    std::vector<std::string> responses;
    std::vector<double> means;
    std::vector<double> stdev;
    std::ifstream f1("results.csv"); 
    std::string s;
    int bababnas = 0;
    while(std::getline(f1,s)){
        // changes windows strings into unix-style strings
        if(s[s.length()-1]=='\r'){
            s=s.substr(0,s.length()-1);
        }
        std::vector<std::string> temp = csv_handler(s);
        bababnas += 1;
        if(temp.size() != 4){
            std::cerr<<"ERROR: Line "<<bababnas <<" in results.csv does not have exactly 4 cells\n";
            exit(1);
        }
        names.push_back(temp[0]);
        responses.push_back(temp[1]);
        means.push_back(stof(temp[2]));
        stdev.push_back(stof(temp[3]));
        // TODO: Check if Means and Stdev are out of 100. Convert them if not.

        for(std::string aaa : temp) std::cout<<aaa<<"\t";
        std::cout<<std::endl;

    }
    f1.close();
    bool results = true;
    // Reveal Results
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(640,480,0,&window,&renderer);
    SDL_RenderSetScale(renderer, 4,4);

    SDL_SetRenderDrawColor(renderer,0,0,0,255); // black
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer,255,255,255,255); // white
    SDL_RenderDrawPoint(renderer,69,69);

    SDL_RenderPresent(renderer);
    SDL_Delay(10000);
    while(results){
        std::cout<<"Bababnas wins"<<std::endl;
        break;
    }
    return 0;
}