#include <algorithm>
#include "User_input.h"

klib::User_input::User_input(void) : mw_counter{ c::MOUSE_WHEEL_DELAY }, mw_y{ 0 } {
	mouse_x = 0;
	mouse_y = 0;
	mouse_state_current = SDL_GetMouseState(&mouse_x, &mouse_y);
	mouse_state_prev = mouse_state_current;
}

bool klib::User_input::is_pressed(SDL_Scancode p_key) const {
	return pressed_keys.find(p_key) != end(pressed_keys);
}

void klib::User_input::move(int delta_ms, int p_mw_y, float p_scale_x, float p_scale_y) {
	pressed_keys.clear();
	this->mw_y = 0;
	auto keyboard_state_current = SDL_GetKeyboardState(nullptr);

	mouse_state_prev = mouse_state_current;
	mouse_state_current = SDL_GetMouseState(&mouse_x, &mouse_y);
	this->mouse_x = static_cast<int>(this->mouse_x / p_scale_x);
	this->mouse_y = static_cast<int>(this->mouse_y / p_scale_y);

	for (const auto& key : klib::c::KEY_DELAYS) {

		int time_left = std::max<int>(0, key_times[key.first] - static_cast<int>(delta_ms));
		key_times[key.first] = time_left;

		if (time_left == 0 && keyboard_state_current[key.first]) {
			pressed_keys.insert(key.first);
			key_times[key.first] = klib::c::KEY_DELAYS.at(key.first);
		}
	}

	// handle mouse wheel
	this->mw_counter -= delta_ms;

	if (mw_counter <= 0 && p_mw_y != 0) {
		this->mw_y = p_mw_y;
		mw_counter = c::MOUSE_WHEEL_DELAY;
	}
	else
		this->mw_counter = std::max(this->mw_counter, 0);
}


bool klib::User_input::mouse_clicked(bool left_b) const {
	auto check_button = (left_b ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT);
	return (mouse_state_prev & SDL_BUTTON(check_button)) && !(mouse_state_current & SDL_BUTTON(check_button));
}

bool klib::User_input::mouse_held(bool left_b) const {
	auto check_button = (left_b ? SDL_BUTTON_LEFT : SDL_BUTTON_RIGHT);
	return (mouse_state_current & SDL_BUTTON(check_button));
}

int klib::User_input::mx(void) const {
	return this->mouse_x;
}

int klib::User_input::my(void) const {
	return this->mouse_y;
}

bool klib::User_input::is_shift_pressed(void) const {
	return this->is_pressed(SDL_SCANCODE_LSHIFT) || this->is_pressed(SDL_SCANCODE_RSHIFT);
}

bool klib::User_input::is_ctrl_pressed(void) const {
	return this->is_pressed(SDL_SCANCODE_LCTRL) || this->is_pressed(SDL_SCANCODE_RCTRL);
}

bool klib::User_input::mw_up(void) const {
	return (this->mw_y > 0);
}

bool klib::User_input::mw_down(void) const {
	return (this->mw_y < 0);
}
