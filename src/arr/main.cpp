#include "timer.hpp"

#include "ui.hpp"

#include "sdlxx.hpp"

int main()
{
    auto init = sdl::Init{SDL_INIT_AUDIO | SDL_INIT_VIDEO};

    auto window = sdl::Window{"ARR", 1024, 768, SDL_WINDOW_FULLSCREEN};
    auto renderer = sdl::Renderer{window};

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

            renderer.present();

        }

        timer.relax();
    }
}
