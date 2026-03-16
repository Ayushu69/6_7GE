#include <SDL2/SDL.h>
#include <bits/stdc++.h>

using namespace std;

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
    SDL_FRect player;
    player.x = 100;
    player.y = 100;
    player.w = 40;
    player.h = 40;

    float speed = 300.0f; // pixels per second

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
        deltaTime /= 1000.0; // ms -> seconds

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

        if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
            player.y -= speed * (float)deltaTime;

        if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
            player.y += speed * (float)deltaTime;

        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
            player.x -= speed * (float)deltaTime;

        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
            player.x += speed * (float)deltaTime;

        // -------------------------
        // Optional: Keep player inside window
        // -------------------------
        if (player.x < 0) player.x = 0;
        if (player.y < 0) player.y = 0;
        if (player.x + player.w > 800) player.x = 800 - player.w;
        if (player.y + player.h > 600) player.y = 600 - player.h;

        // -------------------------
        // Render
        // -------------------------
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &player);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


// #include <SDL2/SDL.h>
// #include <iostream>

// int main(int argc, char* argv[]) {
//     if (SDL_Init(SDL_INIT_VIDEO) != 0) {
//         std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
//         return 1;
//     }

//     SDL_Window* window = SDL_CreateWindow(
//         "six_sevenGE",
//         SDL_WINDOWPOS_CENTERED,
//         SDL_WINDOWPOS_CENTERED,
//         800, 600,
//         SDL_WINDOW_SHOWN
//     );

//     if (!window) {
//         std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << "\n";
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(
//         window,
//         -1,
//         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
//     );

//     if (!renderer) {
//         std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << "\n";
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     bool running = true;
//     SDL_Event event;

//     while (running) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) running = false;
//         }

//         // clear screen (dark gray)
//         SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
//         SDL_RenderClear(renderer);

//         // draw a red rectangle
//         SDL_Rect playerRect = {350, 250, 100, 100};
//         // static int x = 0;
//         // x += 1;
//         // SDL_Rect playerRect = {x, 250, 100, 100};

//         SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
//         SDL_RenderFillRect(renderer, &playerRect);

//         // show it>
//         SDL_RenderPresent(renderer);
//     }

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();
//     return 0;
// }
