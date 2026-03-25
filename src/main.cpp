#include <SDL2/SDL.h>
#include <bits/stdc++.h>

using namespace std;

struct Player{
    SDL_FRect rect;
    float velX = 0.0f, velY = 0.0f;
    float acceleration, friction, maxSpeed;
    float dx, dy;


    void update(const Uint8* keys, float deltaTime){
        dx = dy = 0.0f;

        if(keys[SDL_SCANCODE_W]) dy -= 1;
        if(keys[SDL_SCANCODE_S]) dy += 1;
        if(keys[SDL_SCANCODE_A]) dx -= 1;
        if(keys[SDL_SCANCODE_D]) dx += 1;

        // acceleration
        if(dx != 0) velX += dx*acceleration*deltaTime;
        if(dy != 0) velY += dy*acceleration*deltaTime;

        // friction
        if(dx == 0){
            if(velX > 0){
                velX -= friction * deltaTime;
                if(velX < 0) velX = 0;
            }
            else if(velX < 0){
                velX += friction * deltaTime;
                if(velX > 0) velX = 0;
            }
        }
        if(dy == 0){
            if(velY > 0){
                velY -= friction * deltaTime;
                if(velY < 0) velY = 0;
            }
            else if(velY < 0){
                velY += friction * deltaTime;
                if(velY > 0) velY = 0;
            }
        }

        //  speed clamp
        if(velX > maxSpeed) velX = maxSpeed;
        if(velX < -maxSpeed) velX = -maxSpeed;

        if(velY > maxSpeed) velY = maxSpeed;
        if(velY < -maxSpeed) velY = -maxSpeed;

        //move
        rect.x += velX * deltaTime;
        rect.y += velY * deltaTime;

        // position clamp
        if (rect.x < 0) {
            rect.x = 0;
            velX = 0;
        }
        if (rect.y < 0) {
            rect.y = 0;
            velY = 0;
        }
        if (rect.x + rect.w > 800) {
            rect.x = 800 - rect.w;
            velX = 0;
        }
        if (rect.y + rect.h > 600) {
            rect.y = 600 - rect.h;
            velY = 0;
        }
    }

    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &rect);
    }
};

struct Box{
    SDL_FRect rect;

    void render(SDL_Renderer* renderer) const{
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRectF(renderer, &rect);
    }
};

bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

class Game {
public:
    Player player;
    vector<Box> boxes;

    Game(){
        player.rect = {100, 100, 50, 50};
        player.acceleration = 2000.0f;
        player.friction = 1200.0f;
        player.maxSpeed = 400.0f;
        
        boxes.push_back({{300, 200, 100, 100}});
        boxes.push_back({{500, 100, 150, 50}});
        boxes.push_back({{200, 400, 200, 50}});
    }

    void update(const Uint8* keys, float deltaTime) {
        player.update(keys, deltaTime);

        // COLLISION RESOLUTION
        for (const auto& box : boxes) {

            if (checkCollision(player.rect, box.rect)) {

                float overlapLeft   = (player.rect.x + player.rect.w) - box.rect.x;
                float overlapRight  = (box.rect.x + box.rect.w) - player.rect.x;
                float overlapTop    = (player.rect.y + player.rect.h) - box.rect.y;
                float overlapBottom = (box.rect.y + box.rect.h) - player.rect.y;

                float minX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
                float minY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

                // resolve on smallest axis
                if (minX < minY) {
                    if (overlapLeft < overlapRight)
                        player.rect.x -= overlapLeft;
                    else
                        player.rect.x += overlapRight;

                    player.velX = 0;
                } else {
                    if (overlapTop < overlapBottom)
                        player.rect.y -= overlapTop;
                    else
                        player.rect.y += overlapBottom;

                    player.velY = 0;
                }
            }
        }
    }

    void render(SDL_Renderer* renderer) {
        for(const auto& box: boxes){
            box.render(renderer);
        }
        player.render(renderer);
    }
};


// EVENTS
void handleEvents(bool& running){
    SDL_Event event;

    while (SDL_PollEvent(&event)){
        if (event.type == SDL_QUIT) running = false;
        
        if (event.type == SDL_KEYDOWN){
            if(event.key.keysym.sym == SDLK_ESCAPE) running = false;
        }
    }
}

// GAME UPDATE
void tick(Game& game, float deltaTime){
    SDL_PumpEvents();
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    game.update(keys, deltaTime);
}

// GLOBAL RENDERER
void render(SDL_Renderer* renderer, Game& game) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    game.render(renderer);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "six_sevenGE",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!window)
    {
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer)
    {
        cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    Game game;

    // Timing for DT
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0;

    // FPS counter
    int frames = 0;
    double fpsTimer = 0.0;

    while (running)
    {
        // Delta time
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)(now - last) / SDL_GetPerformanceFrequency();

        fpsTimer += deltaTime;
        frames++;

        if (fpsTimer >= 1.0)
        {
            cout << "FPS: " << frames << "\n";
            frames = 0;
            fpsTimer = 0.0;
        }

        // GAME SYSTEM (EVENTS, UPDATION, RENDERING)
        handleEvents(running);
        tick(game, deltaTime);
        render(renderer, game);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}