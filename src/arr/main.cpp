#include "timer.hpp"

#include "ui.hpp"

#include "assets.hpp"
#include "sdlxx.hpp"


int main()
{
    auto init = sdl::Init{SDL_INIT_AUDIO | SDL_INIT_VIDEO};

    auto window = sdl::Window{"ARR", 1024, 768, SDL_WINDOW_FULLSCREEN};
    auto renderer = sdl::Renderer{window};

    auto sheetTexture = renderer.loadTexture(assets::sheet);

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
                sheetTexture,
                SDL_FRect{.x=0, .y=0, .w=32, .h=32},
                SDL_FRect{.x=10, .y=10, .w=32, .h=32});

            renderer.present();
        }

        timer.relax();
    }
}
