#include "timer.hpp"

#include "assets.hpp"
#include "ui.hpp"

#include "sdlxx.hpp"

int main()
{
    auto init = sdl::Init{SDL_INIT_AUDIO | SDL_INIT_VIDEO};

    auto window = sdl::Window{"ARR", 1024, 768, 0};
    auto renderer = sdl::Renderer{window};
    assets::init(renderer);

    bool done = false;

    auto ui = UI{};
    ui.emplace<Button>(assets::sprites::closeButton)
        ->origin(100, 100)
        ->click(
            [&done]
            {
                done = true;
            });

    auto timer = FrameTimer{60};
    for (;;) {
        for (auto e = SDL_Event{}; SDL_PollEvent(&e) && !done;) {
            if (e.type == SDL_EVENT_QUIT) {
                done = true;
            } else {
                ui.processEvent(e);
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
