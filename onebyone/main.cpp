#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
// compile with g++ -Wall -Wextra -g main.cpp -lSDL2 -lSDL2_ttf -ldl
// Run with ./a.out

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
    for(unsigned long i =0; i<s.length(); i++){
        char c = s[i];
        if(quoted){
            if(c == '"'){
                if(s[i+1] == '"'){
                    
                    buffer += '"';
                    i++;
                    continue;
                }
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
    // Debug code for later
    std::cout<<"argc = "<<argc<<std::endl;
    std::cout<<"argv = [";
    for(int i =0; i<argc; i++){
        std::cout<<argv[i];
        if(i+1 == argc){
            std::cout<<"]";
        }
        else{
            std::cout<<",";
        }
    }
    std::cout<<std::endl;
    /* Rules about Rules
       - The CSV must be in the order you want reveals to be in
       - The line format is name, response, Mu, Sigma
       - DRPs and Dummies are not supported at this time.
       - NO SMART QUOTES (alt codes 0147 and 0148). Not in ASCII, so don't even try
       - NO SINGLE CHARACTER ELLIPSES (alt code 0133). You can use them in the TWOW on character limit rounds, but I'll rectify them when making 1b1s.
       TODO: Integrate this all with a vote parser 
     */
    std::vector<std::string> names;
    std::vector<std::string> responses;
    std::vector<double> means;
    std::vector<double> stdev;
    std::ifstream f1("results.csv"); 
    std::string s;
    // NOTE: Change the prompt when necessary.
    std::string prompt_string2 = "You are a ___fighter. You're similar to a firefighter, but instead of fighting fire, you fight some other thing.  What thing do you fight, and how do you do it effectively? (The thing you fight can be anything, it doesn't have to help society. Just think of yourself as some crazy person who wants to eliminate all of this thing to the best of his abilities)		";
    auto prompt_string = prompt_string2.c_str();
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
            std::cout<<temp[1]<<std::endl;
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

    // Setup SDL2_TTF and the font
    if(TTF_Init() == -1){
        std::cout<<TTF_GetError()<<std::endl;
        return 1;
    }
    TTF_Font* our_font = TTF_OpenFont("Overpass-Regular.ttf",16);
    TTF_Font* big_font = TTF_OpenFont("Overpass-Regular.ttf",32);
    if(our_font == nullptr){
        std::cout<<TTF_GetError()<<std::endl;
        return 1;
    }

   // auto text_surface = TTF_RenderText_Blended_Wrapped(our_font,"Did you know that the mitochondria is the powerhouse of the cell?",{0,0,0},250);
    // I love how text wrapping is 100% not covered by the documentation
   // auto text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
   // SDL_FreeSurface(text_surface);
   



    // TODO: create some sort of antialiased arrow soon, it looks like ass
   // auto red_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,420,69);
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
    SDL_FreeSurface(window_surface);
    std::cout<<"Line 140: "<<SDL_GetError()<<std::endl;
    if(rc < 0){
        std::cerr<<SDL_GetError();
        exit(1);
    }
    // Response Texture Handler: Inside Input Event Handler
    std::vector<SDL_Rect> small_books;
    std::vector<SDL_Texture*> small_book_textures;
    SDL_Texture* current_book = nullptr;
    SDL_Surface* temp_surface = nullptr;

    // NOTE: If you want to change thresholds, do it manually.
    int prize = (names.size()+5)/10;
    int die = (names.size()+2)/5;
    if(names.size() == 2){
        // Create a DRP/TRP friendly version of this
    }
    std::vector<int> y_values;

    // Keep the prompt in memory
    auto prompt_surface = TTF_RenderText_Blended_Wrapped(our_font,prompt_string,{0,0,0},500);
    auto prompt_texture = SDL_CreateTextureFromSurface(renderer,prompt_surface);
    int prompt_height = prompt_surface->h;
    SDL_Rect prompt{710,10,500,prompt_height};
    SDL_FreeSurface(prompt_surface);

    // Data for responses stored here
    SDL_Texture* text_texture = nullptr;
    SDL_Surface* text_surface = nullptr;
    SDL_Rect response{965,0,250,1};
    int another_height = 1;
    // Big Guy
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
                         std::cout<<"Line 199: "<<SDL_GetError()<<std::endl;
                        if(slide/3 == (int)(names.size())) results = false;
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
                        // Booksonas should be named <integer>.bmp in the order you want the reveal to happen, from 0 to n-1
                        std::cout<<"Line 255: "<<SDL_GetError()<<std::endl;
                        std::cout<<filename<<std::endl;

                        temp_surface = SDL_LoadBMP(filename.c_str());
                        if(temp_surface == nullptr){
                            std::cout<<"ERROR: "<<filename<<" failed to load, using fallback"<<std::endl;
                            temp_surface = SDL_LoadBMP("fallback.bmp");
                        }
                        current_book = SDL_CreateTextureFromSurface(renderer,temp_surface);
                        if((int)small_book_textures.size()<= slide/3) small_book_textures.push_back(current_book);

                        // Load New Response if applicable 
                        std::cout<< "Line 267: "<<responses[slide/3]<<std::endl;
                        text_surface = TTF_RenderText_Blended_Wrapped(our_font,responses[slide/3].c_str(),{0,0,0},250);
                        text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
                        another_height = text_surface->h;
                        response.y = 360-(another_height)/2;
                        response.h = another_height;
                        
                        break;
                    
                    case SDLK_LEFT:
                        std::cout<<"Line 271: "<<SDL_GetError()<<std::endl;
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
        std::cout<<"Line 284: "<<SDL_GetError()<<std::endl;
        // Render Left Half of the screen
        

        // Danger-Prize-Safe Zones
        SDL_Rect red_region{0,720,640,720};
        SDL_Rect green_region{0,0,640,0};
        SDL_Rect prize_region{0,0,640,0};
       // Render Them First
        // Green
        int final_green_h;
        if((int)y_values.size() >= die && y_values.size() < names.size()){
            int temp_index = y_values.size()-die;
            green_region.h=y_values[temp_index];
            if(y_values.size()+1==names.size()){
                final_green_h = green_region.h;
            }
            SDL_SetRenderDrawColor(renderer,172,234,134,127);
            SDL_RenderFillRect(renderer,&green_region);
        }
        else if(y_values.size() == names.size()){
            green_region.h=final_green_h;
            SDL_SetRenderDrawColor(renderer,172,234,134,127);
            SDL_RenderFillRect(renderer,&green_region);
            
        }
        // Red
        if(y_values.size()+die >= names.size() && y_values.size() < names.size()){
            int temp_index = names.size()-die-1;
            red_region.y = y_values[temp_index];
            SDL_SetRenderDrawColor(renderer,223,105,105,127);
            SDL_RenderFillRect(renderer,&red_region);
        }
        else if(y_values.size() == names.size()){
            red_region.y = final_green_h;
            SDL_SetRenderDrawColor(renderer,223,105,105,127);
            SDL_RenderFillRect(renderer,&red_region);
        }
        // Prize
        int final_prize_h;
        if(y_values.size()+prize >= names.size() && y_values.size() < names.size()){
            int temp_index = y_values.size()+prize-names.size();
            prize_region.h = y_values[temp_index];
            if(y_values.size() +1 == names.size()){
                final_prize_h = prize_region.h;
            }
            SDL_SetRenderDrawColor(renderer,245,186,122,127);
            SDL_RenderFillRect(renderer,&prize_region);
        }
        else if(y_values.size() == names.size()){
            prize_region.h = final_prize_h;
            SDL_SetRenderDrawColor(renderer,245,186,122,127);
            SDL_RenderFillRect(renderer,&prize_region);
        }

        // Left-Right Bars
        SDL_Rect yellow_1{128,0,36,720};
        SDL_Rect yellow_2{512,0,36,720};
        SDL_SetRenderDrawColor(renderer,255,255,0,255); // Yellow
        SDL_RenderFillRect(renderer, &yellow_1);
        SDL_RenderFillRect(renderer, &yellow_2);

        // All Gridlines and Mile Marker numbers
        std::vector<SDL_Rect> gridlines;
        std::vector<SDL_Rect> mile_markers;
        SDL_SetRenderDrawColor(renderer,0,0,0,0); // Black
        for(int i =90; i<720; i+=90){ // Horizontal and MU
            SDL_Rect temp{112,i,452,1};
            gridlines.push_back(temp);
            SDL_Rect temp2{570,i-16,32,32};
            mile_markers.push_back(temp2);
        }
        for(int i = 238; i<512; i+=100){ // Vertical and SIGMA (hi tarun)
            SDL_Rect temp{i,0,1,675};
            gridlines.push_back(temp);
            SDL_Rect temp2{i-16,681,32,32};
            mile_markers.push_back(temp2);
        }
        for(SDL_Rect rx : gridlines){ // Fill 'em up
            SDL_RenderFillRect(renderer,&rx);
        }
        const int numbers_to_draw[10] = {80,70,60,50,40,30,20,24,27,30};  // NOTE: edit the last 3 to represent the STDEV boundaries
        for(int i =0; i<10; i++){ // Number Loop
            std::string temp_string = std::to_string(numbers_to_draw[i]);
            auto number_surface = TTF_RenderText_Blended(big_font,temp_string.c_str(),{0,0,0});
            auto number_texture = SDL_CreateTextureFromSurface(renderer,number_surface);
            mile_markers[i].w = number_surface->w;
            mile_markers[i].h = number_surface->h;
            SDL_RenderCopy(renderer,number_texture,NULL,&(mile_markers[i]));
            SDL_FreeSurface(number_surface);
        }

        // I love random segfaults, sometimes you need to hold right at startup
       std::cout<<"Line 379: "<<SDL_GetError()<<std::endl;

        // Render Booksona
        // All file management is held in the input events
        SDL_Rect booksona{705,235,250,250};
        SDL_RenderCopy(renderer,current_book,NULL,&booksona);
       std::cout<<"Line 385: "<<SDL_GetError()<<std::endl;
        
        
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
            temp.x = stdev_to_x(stdev[slide/3],24,30)-24; // NOTE: Edit the left and right boundaries as you see fit
            std::cout<<temp.x+24<<std::endl;
            temp.y = percent_to_y(means[slide/3])-24;
            // Only add a book to the vector if absolutely necessary
            // And you get a red/green region update ABSOLUTELY FREE!
            if((int)small_books.size() <= slide/3){ 
                small_books.push_back(temp);
                y_values.push_back(percent_to_y(means[slide/3]));
            }
            arrow.y = percent_to_y(means[slide/3])-22;

        }  
        std::cout<<"Line 412: "<<SDL_GetError()<<std::endl;


        
        

        // Render Small Booksonas using small_book_textures array
        int jej = 0;
        for(SDL_Rect rx : small_books){
            SDL_RenderCopy(renderer,small_book_textures[jej],NULL,&rx);
            jej+= 1;
        }
        std::cout<<"Line 424: "<<SDL_GetError()<<std::endl;
        // Render Moveable Debug Object
        SDL_SetRenderDrawColor(renderer,0,0,0,0); // Black
        // SDL_RenderFillRect(renderer,&r);


        // TTF: Render Text
        // Render Prompt, UTH is on line 209
        SDL_RenderCopy(renderer,prompt_texture,NULL,&prompt);
        

        // Render Response
        // NOTE: Render before freeing the surface so you can play with the height/width
        SDL_RenderCopy(renderer,text_texture,NULL,&response);
        

        // Render Percentage on slide 3n+2
        if(slide % 3 == 2){
            
            char current_mu[100];
            snprintf(current_mu,100,"%.2f%%",means[slide/3]); // FOCUS
            auto mu_surface = TTF_RenderText_Blended(big_font,current_mu,{0,0,0});
            auto mu_texture = SDL_CreateTextureFromSurface(renderer,mu_surface);
            SDL_Rect big_mu{960-((mu_surface->w)/2),360,mu_surface->w,mu_surface->h}; // TODO: Make this move with the arrow a la CaryTWOW
            big_mu.y = 600 - ((mu_surface->h)/2);
            SDL_RenderCopy(renderer,mu_texture,NULL,&big_mu);
            SDL_FreeSurface(mu_surface);
        }
        
        // Left Side Numbers: See section immediately before arrow




        SDL_RenderPresent(renderer);
        std::cout<<"Line 338: "<<SDL_GetError()<<std::endl;
        SDL_Delay(5);
        
    }
    SDL_DestroyWindow(window);
    //SDL_DestroyTexture(text_texture);
    TTF_CloseFont(our_font);
    TTF_CloseFont(big_font);
    SDL_Quit();
    return 0;
}