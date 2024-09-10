#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdlib>
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
    srand(time(NULL));
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
    SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Rect r{10,10,250,250};
    
    SDL_CreateWindowAndRenderer(1280,720,0,&window,&renderer);
    SDL_RenderSetScale(renderer, 1,1);

    auto red_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,420,69);
    SDL_Surface* image = nullptr;
    image = SDL_LoadBMP("booksonas/00.bmp");
    if(image == nullptr){
        std::cerr<<SDL_GetError();
        return 1;
    }
    auto image_texture = SDL_CreateTextureFromSurface(renderer,image);
    SDL_SetRenderTarget(renderer,image_texture);
    SDL_RenderClear(renderer);
   // SDL_SetRenderDrawColor(renderer, 255,0,0,255);
   // SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,image_texture,nullptr,nullptr);
    SDL_RenderPresent(renderer);
    // TODO: Resave all booksonas as BMP files and pitting them in the booksonas/ directory
    int slide = 0;
    SDL_SetRenderTarget(renderer,nullptr);
    SDL_Delay(1001);

    // Render Scaled Booksona, Remember to adjust r.x and r.y
    SDL_SetRenderDrawColor(renderer,0,0,0,255); // black
    SDL_RenderClear(renderer);
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    int rc = SDL_BlitScaled(image,NULL,window_surface,&r);
    SDL_UpdateWindowSurface( window );
    SDL_Delay(1001);

    if(rc < 0){
        std::cerr<<SDL_GetError();
        exit(1);
    }
    while(results){
        // Input Events
        while(SDL_PollEvent(&e)){

            if(e.type == SDL_QUIT){ // Imagine quitting out of results, lol
                results = false;
            }
            else if(e.type ==SDL_KEYDOWN){ // Increment slides
                switch(e.key.keysym.sym)
                {
                    case SDLK_RIGHT: 
                        r.x += 4;
                        slide++;
                        break;
                    case SDLK_LEFT:
                        r.x -=3;
                        slide--;
                        break;
                }
            }
            else if(e.type ==SDL_KEYUP){
                switch(e.key.keysym.sym){
                    case SDLK_RIGHT:
                        break;
                    case SDLK_LEFT:
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255); // black
        SDL_RenderClear(renderer);

        // Render Moveable Rect Object
        SDL_SetRenderDrawColor(renderer,255,255,255,255); // white
        SDL_RenderFillRect(renderer,&r);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
        
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}