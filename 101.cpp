#include<iostream>
#include<SDL3/SDL.h>
#include<cmath>
using namespace std;
bool gameIsRunning = false;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
int w = 800;
int h = 600;
float r = 100;

bool initialize_window(void){
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cout<<"Failed to initialize"<<endl;
        return false;
    };
    window = SDL_CreateWindow("Task 101", w, h, 0);
if(!window){
    cout<<"Error: Failed to open window\nSDL Error\n";
    return false;
}

    renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer){
        cout<<"Error:Failed to initialize renderer\nSDL erroe\n";
        return false;
    }
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

void render(void){
    SDL_SetRenderDrawColor(renderer, 255, 209, 220, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    drawcircle(w / 2.0f, h / 2.0f, r);

    SDL_RenderPresent(renderer);
}

void destroyWin(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(){
    gameIsRunning = initialize_window();
    while(gameIsRunning){
        process_input();
        render();
    }
    destroyWin();

}