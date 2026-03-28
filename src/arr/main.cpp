#include "timer.hpp"

#include "ui.hpp"

#include "assets.hpp"
#include "sdlxx.hpp"

#include <iostream>

int main()
{
    auto init = sdl::Init{SDL_INIT_AUDIO | SDL_INIT_VIDEO};

    auto window = sdl::Window{"ARR", 1024, 768, SDL_WINDOW_FULLSCREEN};
    auto renderer = sdl::Renderer{window};

    auto sheetBytes =
        std::as_bytes(std::span(assets::sheet, sizeof(assets::sheet)));
    auto sheetTexture = renderer.loadTexture(sheetBytes);

    auto ui = UI{};

    auto timer = FrameTimer{60};
    for (;;) {
        bool done = false;
        for (auto e = SDL_Event{}; SDL_PollEvent(&e) && !done;) {
            if (e.type == SDL_EVENT_QUIT) {
                done = true;
            }
        }
        if (done) {
            break;
        }

        if (auto framesPassed = timer(); framesPassed > 0) {
            for (int i = 0; i < framesPassed; i++) {
                // update
            }

            renderer.drawColor(200, 200, 200);
            renderer.clear();

            ui.present(renderer);

            renderer.render(
                sheetTexture, SDL_FRect{0, 0, 32, 32}, SDL_FRect{10, 10, 32, 32});

            renderer.present();
        }

        timer.relax();
    }
}
