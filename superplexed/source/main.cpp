#include <iostream>
#include <vector>
#include "./sp_gfx/Project_gfx.h"

using byte = unsigned char;

int main(int argc, char* args[]) {
	SDL_Window* l_window{ nullptr };
	SDL_Renderer* l_rnd{ nullptr };
	bool l_exit{ false };

	float l_scale{ 1.0f };

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError();
	else {
		// Event handler
		SDL_Event e;

		l_window = SDL_CreateWindow("Superplexed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
		if (l_window == nullptr)
			std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError();
		else {
			l_rnd = SDL_CreateRenderer(l_window, -1, SDL_RENDERER_ACCELERATED);

			if (l_rnd == nullptr)
				std::cerr << "Renderer could not be created!SDL Error: " << SDL_GetError();
			else {
				//Initialize renderer color
				SDL_SetRenderDrawColor(l_rnd, 0x00, 0x00, 0x00, 0x00);
			}

			// load resources
			Project_gfx p_gfx(l_rnd);

			// main window object to handle all logic and drawing
			//kkit::Main_window main_window(l_rnd, !project.is_klab_v_1() && !project.is_walken());

			// input handler
			//klib::User_input input;
			int mouse_wheel_y{ 0 };
			bool mw_used{ false };

			uint32_t last_logic_time = SDL_GetTicks() - 1;
			uint32_t last_draw_time = SDL_GetTicks() - 17;

			uint32_t delta = 1;
			uint32_t deltaDraw = 17;

			while (!l_exit) {

				uint32_t tick_time = SDL_GetTicks();	// current time

				deltaDraw = tick_time - last_draw_time;
				delta = tick_time - last_logic_time;
				int32_t mw_y{ 0 };

				mw_used = false;
				SDL_PumpEvents();

				if (SDL_PollEvent(&e) != 0)
					if (e.type == SDL_QUIT)
						l_exit = true;
					else if (e.type == SDL_MOUSEWHEEL) {
						mw_used = true;
						mouse_wheel_y = e.wheel.y;
					}

				if (delta != 0) {
					uint32_t realDelta = std::min(delta, 5u);

					//input.move(realDelta, mw_used ? mouse_wheel_y : 0, l_scale, l_scale);
					//main_window.move(l_rnd, input, realDelta, project, p_gfx);

					last_logic_time = tick_time;
				}

				if (deltaDraw >= 25) { // capped frame rate of ~40 is ok
					//mainwindow.draw(input, &texture_manager);
					last_draw_time = SDL_GetTicks();

					//main_window.draw(l_rnd, input, project, p_gfx);

					//Update screen
					SDL_RenderPresent(l_rnd);
				}

				SDL_Delay(1);

			}
		}


	}

	// Destroy window
	SDL_DestroyWindow(l_window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}
