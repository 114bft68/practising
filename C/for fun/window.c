#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define BLACK 0, 0, 0, 255
#define RED   255, 0, 0, 255

struct w
{
    SDL_Window   *window;
    SDL_Renderer *renderer;
};

int main(void)
{
    struct w window = { 0 };
    SDL_Event event = { 0 };

    if (SDL_Init(SDL_INIT_VIDEO) < 0                                                                           ||
        !(window.window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 300, 300,
                                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED))                       ||
        !(window.renderer = SDL_CreateRenderer(window.window, -1, 0)))
    {
        puts("\ninitialization failed");
        return 1;
    }

    SDL_Rect rect = { 0, 0, 200, 200 };

    int running = 1;
    while (running)
    {
        SDL_PollEvent(&event);
        
        switch (event.type)
        {
            case SDL_QUIT:
                SDL_DestroyWindow(window.window);
                SDL_DestroyRenderer(window.renderer);
                return 0;
        }

        SDL_RenderClear(window.renderer);
    
        SDL_SetRenderDrawColor(window.renderer, RED);
        SDL_RenderFillRect(window.renderer, &rect);
        
        SDL_RenderPresent(window.renderer);
    }
    
    return 0;
}