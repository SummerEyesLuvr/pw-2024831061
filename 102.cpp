#include<SDL3/SDL.h>
#include<cmath>
#include<algorithm>
#include<iostream>

using namespace std;

int w= 800;
int h = 600;
float ri= 20.0f;
float s = 120.0f;   
SDL_Window*   window    = nullptr;
SDL_Renderer* renderer  = nullptr;
float r = ri;
float x = 0.0f;
float y = 0.0f;
float rm = 0.0f;
Uint64  t = 0;
bool gameIsRunning = false;

bool initialize_window(void){
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cout<<"Failed to initialize"<<endl;
        return false;
    };
    window = SDL_CreateWindow("Task 102", w, h, 0);
if(!window){
    cout<<"Error: Failed to open window\nSDL Error\n";
    return false;
}

    renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer){
        cout<<"Error:Failed to initialize renderer\nSDL error\n";
        return false;
    }
    x = w/ 2.0f;
    y = h/ 2.0f;
    rm = min(x,y); 
    t = SDL_GetTicks();
    return true;

}

void process_input(void){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_EVENT_QUIT){
            gameIsRunning = false;
            break;
        }
    }
}

void drawcircle(float x, float y, float r){
    float step = 1.0f / r;
    for(float angle = 0; angle < 2 * M_PI; angle+=step){
        SDL_RenderPoint(renderer, x + r * cos(angle), y+ r*sin(angle));
    }
}

void increase(void){
    Uint64 fin = SDL_GetTicks();
    float g = (fin - t) / 1000.0f;
    t = fin;
    r+=g*s;  
    if(r >= rm){
        r = ri;
    }
}

void render(void){
    SDL_SetRenderDrawColor(renderer, 150, 123, 182, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    drawcircle(x,y,r);

    SDL_RenderPresent(renderer);
}

void destroyWin(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(){
    gameIsRunning = initialize_window();
    SDL_Event event;
    while(gameIsRunning){
     process_input();
        increase();
        render();
    }
    destroyWin();
    return 0;
}



