#include <SDL2/SDL.h>
#include <bits/stdc++.h>

using namespace std;

struct Player{
    SDL_FRect rect;
    float speed;

    void update(const Uint8* keys, float deltaTime){
        float dx = 0.0f, dy = 0.0f;

        if(keys[SDL_SCANCODE_W]) dy -= 1;
        if(keys[SDL_SCANCODE_S]) dy += 1;
        if(keys[SDL_SCANCODE_A]) dx -= 1;
        if(keys[SDL_SCANCODE_D]) dx += 1;

        float length = sqrt(dx*dx + dy*dy);
        if(length != 0){
            dx /= length;
            dy /= length;
        }

        rect.x += dx*speed*deltaTime;
        rect.y -= dy*speed*deltaTime;

        if (rect.x < 0) rect.x = 0;
        if (rect.y < 0) rect.y = 0;
        if (rect.x + rect.w > 800) rect.x = 800 - rect.w;
        if (rect.y + rect.h > 600) rect.y = 600 - rect.h;
    }

    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &rect);
    }

};

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cout << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "six_sevenGE - Day 2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
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

    // -------------------------
    // Game State
    // -------------------------
    // SDL_FRect player;
    // player.x = 100;
    // player.y = 100;
    // player.w = 40;
    // player.h = 40;

    // float speed = 300.0f; pixels per second
    Player player;
    player.rect = {100, 100, 40, 40};
    player.speed = 300.0f;

    bool running = true;
    SDL_Event event;

    // Timing
    Uint64 now = SDL_GetPerformanceCounter();
    Uint64 last = 0;
    double deltaTime = 0;

    // FPS counter
    int frames = 0;
    double fpsTimer = 0.0;

    while (running)
    {
        // -------------------------
        // Delta time
        // -------------------------
        last = now;
        now = SDL_GetPerformanceCounter();
        deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());
        deltaTime /= 1000; // ms -> seconds

        fpsTimer += deltaTime;
        frames++;

        if (fpsTimer >= 1.0)
        {
            cout << "FPS: " << frames << "\n";
            frames = 0;
            fpsTimer = 0.0;
        }

        // -------------------------
        // Events
        // -------------------------
        while (SDL_PollEvent(&event))
        {
            // cout << "Event type: " << event.type << "\n";

            if (event.type == SDL_QUIT)
                running = false;

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    running = false;
            }
        }

        // -------------------------
        // Input (real-time)
        // -------------------------
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player.update(keys, deltaTime);

        // if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
        //     player.y -= speed * (float)deltaTime;

        // if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
        //     player.y += speed * (float)deltaTime;

        // if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        //     player.x -= speed * (float)deltaTime;

        // if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        //     player.x += speed * (float)deltaTime;

        // -------------------------
        // Optional: Keep player inside window
        // -------------------------
        // if (player.x < 0) player.x = 0;
        // if (player.y < 0) player.y = 0;
        // if (player.x + player.w > 800) player.x = 800 - player.w;
        // if (player.y + player.h > 600) player.y = 600 - player.h;

        // -------------------------
        // Render
        // -------------------------
        //clear screen
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        //draw player
        player.render(renderer);

        // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        // SDL_RenderFillRectF(renderer, &player);

        //present
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}