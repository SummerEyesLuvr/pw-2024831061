#include <SDL3/SDL.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

int w= 800;
int h= 600;
int r= 30;
float speed= 4.0f;
float key_speed= 5.0f;
int flash_frame= 20;
int shake_frames= 15;
int shake_mag= 8;
constexpr int max_particles= 32;
SDL_Window   *window   = nullptr;
SDL_Renderer *renderer = nullptr;
bool gameIsRuning  = true;

struct Circle {
    float x, y;
    int   radius;
    SDL_Color color;     
    SDL_Color hitColor;    
};

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    bool  active = false;
};

struct GameState {
    Circle   autoCircle;
    Circle   playerCircle;
    Particle particles[max_particles];
    bool colliding= false;
    int flashTimer= 0;
    int shakeTimer= 0;
    int ox = 0; 
    int oy= 0;   
};

GameState state;

bool initialize_window(void){
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        cout<<"Failed to initialize"<<endl;
        return false;
    };
    window = SDL_CreateWindow("Task 103", w, h, 0);
if(!window){
    cout<<"Error: Failed to open window\nSDL Error\n";
    return false;
}

    renderer = SDL_CreateRenderer(window, nullptr);
    if(!renderer){
        cout<<"Error:Failed to initialize renderer\nSDL error\n";
        return false;
    }
    return true;
}

Circle createCircle(float x, float y, int radius,
                    SDL_Color color, SDL_Color hitColor){
    Circle c;
    c.x = x;
    c.y  = y;
    c.radius = radius;
    c.color = color;
    c.hitColor = hitColor;
    return c;
}

void initGameState(GameState &gs){
    gs.autoCircle = createCircle(
        (float)r,
        (float)(h / 2),
        r,
        SDL_Color{0, 220, 255, 255},    
        SDL_Color{255, 80,  30,  255}   
    );

    gs.playerCircle = createCircle(
        (float)(w / 2),
        (float)r,
        r,
        SDL_Color{80,  255, 120, 255},  
        SDL_Color{255, 30,  80,  255}   
    );

    for (auto &p : gs.particles) p.active = false;

    gs.colliding  = false;
    gs.flashTimer = 0;
    gs.shakeTimer = 0;
    gs.ox = gs.oy = 0;
}

void spawnParticles(GameState &gs, float cx, float cy){
    for (int i = 0; i < max_particles; i++) {
        float angle = (float)i * (2.0f * (float)M_PI / (float)max_particles);
        float speed = 2.0f + (float)(rand() % 3);
        gs.particles[i] = {
            cx, cy,
            cosf(angle) * speed,
            sinf(angle) * speed,
            1.0f, true
        };
    }
}

void updateParticles(GameState &gs, float dt){
    for (auto &p : gs.particles) {
        if (!p.active) continue;
        p.x+= p.vx*dt;
        p.y+= p.vy*dt;
        p.life-=0.04f*dt;
        if (p.life <= 0.0f) p.active = false;
    }
}

void drawFilledCircle(SDL_Renderer *r, int cx, int cy, int radius){
    for (int dy = -radius; dy <= radius; dy++) {
        int dx = (int)sqrtf((float)(radius * radius - dy * dy));
        SDL_RenderLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void drawCircleOutline(SDL_Renderer *r, int cx, int cy, int radius){
    for (int angle = 0; angle < 360; angle++) {
        float rad = (float)angle * (float)M_PI / 180.0f;
        int x = cx + (int)((float)radius * cosf(rad));
        int y = cy + (int)((float)radius * sinf(rad));
        SDL_RenderPoint(r, (float)x, (float)y);
    }
}

void drawCircle(SDL_Renderer *r, const Circle &c, int ox, int oy){
    SDL_SetRenderDrawColor(r, c.color.r, c.color.g, c.color.b, c.color.a);
    drawFilledCircle(r, (int)c.x + ox, (int)c.y + oy, c.radius);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 200);
    drawCircleOutline(r, (int)c.x + ox, (int)c.y + oy, c.radius + 2);
}

void drawParticles(SDL_Renderer *r, const GameState &gs){
    for (const auto &p : gs.particles) {
        if (!p.active) continue;
        Uint8 a = (Uint8)(p.life * 255.0f);
        SDL_SetRenderDrawColor(r, 255, 220, 50, a);
        SDL_FRect dot = {
            p.x + (float)gs.ox - 2.0f,
            p.y + (float)gs.oy - 2.0f,
            5.0f, 5.0f
        };
        SDL_RenderFillRect(r, &dot);
    }
}

void drawCollisionBanner(SDL_Renderer *r){
    SDL_SetRenderDrawColor(r, 255, 0, 0, 180);
    SDL_FRect banner = { 250.0f, 10.0f, 300.0f, 36.0f };
    SDL_RenderFillRect(r, &banner);
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_RenderRect(r, &banner);
}

void process_input(void){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
            gameIsRuning = false;
        if (event.type == SDL_EVENT_KEY_DOWN &&
            event.key.key == SDLK_ESCAPE)
            gameIsRuning = false;
    }
}

void handlePlayerInput(Circle &player, float dt){
    const bool *keys = SDL_GetKeyboardState(nullptr);
    float spd = speed * dt;

    if (keys[SDL_SCANCODE_LEFT]) player.x -= spd;
    if (keys[SDL_SCANCODE_RIGHT]) player.x += spd;
    if (keys[SDL_SCANCODE_UP]) player.y -= spd;
    if (keys[SDL_SCANCODE_DOWN]) player.y += spd;

    float r = (float)player.radius;
    if (player.x < r) player.x = r;
    if (player.x > (float)w - r) player.x = (float)w - r;
    if (player.y < r) player.y = r;
    if (player.y > (float)h - r) player.y = (float)h - r;
}

void updateAutoCircle(Circle &c, float dt){
    c.x += speed * dt;
    if (c.x - (float)c.radius > (float)w)
        c.x = -(float)c.radius;
}

bool checkCollision(const Circle &a, const Circle &b){
    float dx   = a.x - b.x;
    float dy   = a.y - b.y;
    float dist = sqrtf(dx * dx + dy * dy);
    return dist < (float)(a.radius + b.radius);
}

void updateCollision(GameState &gs){
    bool hit = checkCollision(gs.autoCircle, gs.playerCircle);

    if (hit && !gs.colliding) {
        gs.shakeTimer = shake_frames;
        spawnParticles(gs,
            (gs.autoCircle.x + gs.playerCircle.x) * 0.5f,
            (gs.autoCircle.y + gs.playerCircle.y) * 0.5f);
        gs.colliding = true;
    } else if (!hit) {
        gs.colliding = false;
    }
}

void updateShake(GameState &gs)
{
    gs.ox = 0;
    gs.oy = 0;
    if (gs.shakeTimer > 0) {
        gs.ox = (rand() % (shake_mag * 2 + 1)) - shake_mag;
        gs.oy = (rand() % (shake_mag * 2 + 1)) - shake_mag;
        gs.shakeTimer--;
    }
}

void render()
{
    SDL_Renderer *r  = renderer;
    GameState &gs = state;
    SDL_SetRenderDrawColor(r, 139, 45, 87, 255);
    SDL_RenderClear(r);

    drawCircle(r, gs.autoCircle,   gs.ox, gs.oy);
    drawCircle(r, gs.playerCircle, gs.ox, gs.oy);
    drawParticles(r, gs);
    SDL_RenderPresent(r);
}

void destroyWin(void){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    renderer = nullptr;
    window = nullptr;
}

int main(){

    if (!initialize_window())
        return 1;

    initGameState(state);
    Uint64 prev = SDL_GetTicks();

    while (gameIsRuning) {
        Uint64 now = SDL_GetTicks();
        float  dt  = (float)(now - prev) / 20.0f;
        if (dt > 3.0f) dt = 3.0f;
        prev = now;

        process_input();
        handlePlayerInput(state.playerCircle, dt);
        updateAutoCircle(state.autoCircle, dt);
        updateCollision(state);
        updateParticles(state, dt);
        updateShake(state);
        render();

        SDL_Delay(16);
    }
    destroyWin();

    return 0;
}



