#ifndef KLIB_USER_INPUT_H
#define KLIB_USER_INPUT_H

#include <SDL.h>
#include <set>
#include <map>

namespace klib {

	class User_input {
		std::map<SDL_Scancode, int> key_times;
		std::set<SDL_Scancode> pressed_keys;
		int mouse_x, mouse_y, mw_y, mw_counter;
		Uint32 mouse_state_prev, mouse_state_current;

	public:
		User_input(void);
		void move(int p_delta_ms, int p_mw_y, float p_scale_x = 1.0f, float p_scale_y = 1.0f);

		bool is_shift_pressed(void) const;
		bool is_ctrl_pressed(void) const;
		bool is_pressed(SDL_Scancode) const;
		bool mouse_clicked(bool = true) const;
		bool mouse_held(bool = true) const;
		int mx(void) const;
		int my(void) const;

		bool mw_up(void) const;
		bool mw_down(void) const;
	};

	namespace c {
		// mousewheel repeat time (in ms)
		constexpr int MOUSE_WHEEL_DELAY{ 15 };
		// keyboard repeat times (in ms) - a key will not be considered pressed unless X amount of ms have elapsed since last time it was pressed - use 0 ms for things like ctrl/shift etc
		constexpr int DEFAULT_KRV{ 100 }; // the default time a key will have to wait until it is considered re-pressed
		const std::map<SDL_Scancode, int> KEY_DELAYS{ {SDL_SCANCODE_LEFT, DEFAULT_KRV}, {SDL_SCANCODE_DOWN, DEFAULT_KRV}, {SDL_SCANCODE_RIGHT, DEFAULT_KRV}, {SDL_SCANCODE_UP, DEFAULT_KRV}, {SDL_SCANCODE_PAGEDOWN, 2 * DEFAULT_KRV},{SDL_SCANCODE_PAGEUP, 2 * DEFAULT_KRV}, {SDL_SCANCODE_HOME, DEFAULT_KRV},{SDL_SCANCODE_END, DEFAULT_KRV}, {SDL_SCANCODE_KP_MINUS, DEFAULT_KRV}, {SDL_SCANCODE_KP_PLUS, DEFAULT_KRV}, {SDL_SCANCODE_ESCAPE, DEFAULT_KRV}, // movement keys
			{SDL_SCANCODE_C, DEFAULT_KRV }, {SDL_SCANCODE_V, DEFAULT_KRV }, {SDL_SCANCODE_T, 2 * DEFAULT_KRV }, {SDL_SCANCODE_F, 2 * DEFAULT_KRV },
			{SDL_SCANCODE_H, 2 * DEFAULT_KRV },{SDL_SCANCODE_DELETE, DEFAULT_KRV },{SDL_SCANCODE_S, DEFAULT_KRV }, {SDL_SCANCODE_M, DEFAULT_KRV },
			{SDL_SCANCODE_TAB, 2 * DEFAULT_KRV }, {SDL_SCANCODE_H, DEFAULT_KRV }, {SDL_SCANCODE_A, DEFAULT_KRV }, {SDL_SCANCODE_B,2 * DEFAULT_KRV },
			{SDL_SCANCODE_R, 1000}, {SDL_SCANCODE_X, 1000 }, {SDL_SCANCODE_D, DEFAULT_KRV}, {SDL_SCANCODE_N, DEFAULT_KRV}, {SDL_SCANCODE_P, DEFAULT_KRV},
			{SDL_SCANCODE_G, DEFAULT_KRV},
			{SDL_SCANCODE_F1, 3 * DEFAULT_KRV}, // copy/paste etc - other non-control keys
			{SDL_SCANCODE_LSHIFT,0 }, {SDL_SCANCODE_RSHIFT,0 }, {SDL_SCANCODE_LCTRL,0 }, {SDL_SCANCODE_RCTRL,0 }, {SDL_SCANCODE_LALT,0 } // control keys
		};
	}

}

#endif
