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

double y_to_percent(int y){
    return 90.0-(1.0/9.0 * y);
}
int percent_to_y(double percent){
    return (int)(810 - 9* percent);
}
int stdev_to_x(double stdev,int left, int right){
    double slope = 200.0/(right-left);
    return (int)(238+slope*(stdev-left));
}

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
    
    // CSV Parser
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

    // Setup Window and Renderer
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event e;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Rect r{10,10,250,250};
    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    
    SDL_CreateWindowAndRenderer(1280,720,0,&window,&renderer);
    SDL_RenderSetScale(renderer, 1,1);

    // TODO: create some sort of antialiased arrow soon, it looks like ass
    auto red_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,420,69);
    SDL_Surface* image = nullptr;
    image = SDL_LoadBMP("fallback.bmp");
    if(image == nullptr){ // TODO: More Error Checking
        std::cerr<<SDL_GetError();
        return 1;
    }
    auto arrow_surface = SDL_LoadBMP("arrow.bmp");
    auto arrow_texture = SDL_CreateTextureFromSurface(renderer,arrow_surface);
    auto image_texture = SDL_CreateTextureFromSurface(renderer,image);
    SDL_SetRenderTarget(renderer,image_texture);
    SDL_RenderClear(renderer); // All is MineFlex_B
    std::cout<<"Line 120: "<<SDL_GetError()<<std::endl;
   // SDL_SetRenderDrawColor(renderer, 255,0,0,255);
   // SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,image_texture,nullptr,nullptr);
    SDL_RenderPresent(renderer);
    int slide = -1;
    // Slide Number
    // -1: Begin
    // 3n+0: Introduce the contestant 
    // +1: Animate their placement
    // +2: Wait for the next contestant
    SDL_Rect arrow{564,-90,90,45};
    SDL_SetRenderTarget(renderer,nullptr);


    // Render Scaled Booksona, Remember to adjust r.x and r.y
    SDL_SetRenderDrawColor(renderer,255,255,255,255); // white
    SDL_RenderClear(renderer);
    SDL_Surface* window_surface = SDL_GetWindowSurface(window);
    int rc = SDL_BlitScaled(image,NULL,window_surface,&r);
    SDL_UpdateWindowSurface( window );
    std::cout<<"Line 140: "<<SDL_GetError()<<std::endl;
    if(rc < 0){
        std::cerr<<SDL_GetError();
        exit(1);
    }
    std::vector<SDL_Rect> small_books;
    std::vector<SDL_Texture*> small_book_textures;
    SDL_Texture* current_book = nullptr;
    SDL_Surface* temp_surface = nullptr;
    int prize = (names.size()+5)/10;
    int die = (names.size()+2)/5;
    std::vector<int> y_values;
    while(results){
        std::sort(y_values.begin(),y_values.end());
        std::cout<<"Entered loop on slide "<<slide<<std::endl;
        std::cout<<SDL_GetError()<<std::endl;
        std::string filename = "booksonas/"+std::to_string(slide/3)+".bmp";
        
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
                        // std::cout<<"Line 165: "<<SDL_GetError()<<std::endl;
                        if(slide/3 == names.size()) results = false;
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
                        // Load New Booksona if applicable
                        //std::cout<<"Line 179: "<<SDL_GetError()<<std::endl;
                        std::cout<<filename<<std::endl;

                        temp_surface = SDL_LoadBMP(filename.c_str());
                        if(temp_surface == nullptr){
                            std::cout<<"ERROR: "<<filename<<" failed to load, using fallback"<<std::endl;
                            temp_surface = SDL_LoadBMP("fallback.bmp");
                        }
                        current_book = SDL_CreateTextureFromSurface(renderer,temp_surface);
                        if(small_book_textures.size()<= slide/3) small_book_textures.push_back(current_book);
                        break;
                    
                    case SDLK_LEFT:
                        //std::cout<<"Line 191: "<<SDL_GetError()<<std::endl;
                        break;
                }
            }
        }

        // Restart the party
        SDL_SetRenderDrawColor(renderer,255,255,255,255); // White
        SDL_RenderClear(renderer);
        if(slide <0){
            SDL_RenderPresent(renderer);
            continue;
        }
       // std::cout<<"Line 200: "<<SDL_GetError()<<std::endl;
        // Render Left Half of the screen

        // Danger-Prize-Safe Zones
        SDL_Rect red_region{0,720,640,720};
        SDL_Rect green_region{0,0,640,0};
        SDL_Rect prize_region{0,0,640,0};
        // Left-Right Bars
        SDL_Rect yellow_1{128,0,36,720};
        SDL_Rect yellow_2{512,0,36,720};
        SDL_SetRenderDrawColor(renderer,255,255,0,255); // Yellow
        SDL_RenderFillRect(renderer, &yellow_1);
        SDL_RenderFillRect(renderer, &yellow_2);

        // All Gridlines
        std::vector<SDL_Rect> gridlines;
        SDL_SetRenderDrawColor(renderer,0,0,0,0); // Black
        for(int i =90; i<720; i+=90){
            SDL_Rect temp{112,i,452,1};
            gridlines.push_back(temp);
        }
        for(int i = 238; i<512; i+=100){
            SDL_Rect temp{i,0,1,675};
            gridlines.push_back(temp);
        }
        for(SDL_Rect rx : gridlines){
            SDL_RenderFillRect(renderer,&rx);
        }


       // std::cout<<"Line 230: "<<SDL_GetError()<<std::endl;
        // Render Booksona
        // All file management is held in the input events
        SDL_Rect booksona{835,45,250,250};
        SDL_RenderCopy(renderer,current_book,NULL,&booksona);
       //std::cout<<"Line 235: "<<SDL_GetError()<<std::endl;

        // Render Arrow 
        // TODO: create some sort of antialiased arrow soon, it looks like ass
        SDL_RenderCopy(renderer,arrow_texture,NULL,&arrow);
        if(slide % 3 == 1){ // Sliding Downwards
            if(y_to_percent(arrow.y+22)>means[slide/3])arrow.y++;
            else slide++;
        }   
        else if(slide % 3 == 0){ // Back Up Top
            arrow.y = -102;
        }
        else{ // Add Small Book + Lock In Place + Update Red/Green Regions
            SDL_Rect temp{0,0,48,48};
            temp.x = stdev_to_x(stdev[slide/3],21,29)-24;
            std::cout<<temp.x+24<<std::endl;
            temp.y = percent_to_y(means[slide/3])-24;
            // Only add a book to the vector if absolutely necessary
            // And you get a red/green region update ABSOLUTELY FREE!
            if(small_books.size() <= slide/3){ 
                small_books.push_back(temp);
                y_values.push_back(percent_to_y(means[slide/3]));
            }
            arrow.y = percent_to_y(means[slide/3])-22;

        }  
        //std::cout<<"Line 257: "<<SDL_GetError()<<std::endl;


        
        // Render Colored Regions
        // Green
        if(y_values.size() >= die && y_values.size() < names.size()){
            green_region.h=y_values[y_values.size()-die];
            SDL_SetRenderDrawColor(renderer,0,255,0,127);
            SDL_RenderFillRect(renderer,&green_region);
        }
        // Red
        // Prize

        // Render Small Booksonas using small_book_textures array
        int jej = 0;
        for(SDL_Rect rx : small_books){
            SDL_RenderCopy(renderer,small_book_textures[jej],NULL,&rx);
            jej+= 1;
        }
        //std::cout<<"Line 264: "<<SDL_GetError()<<std::endl;
        // Render Moveable Debug Object
        SDL_SetRenderDrawColor(renderer,0,0,0,0); // Black
        // SDL_RenderFillRect(renderer,&r);
        


        SDL_RenderPresent(renderer);
        //std::cout<<"Line 272: "<<SDL_GetError()<<std::endl;
        SDL_Delay(10);
        
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}