#include "Timer.h"

klib::Timer::Timer(int p_max_no, int p_ms_per_frame, bool p_backward) :
	max_no{ p_max_no }, ms_per_frame{ p_ms_per_frame },
	backward{ p_backward }, cur_frame{ 0 }, cur_cnt{ 0 }, is_counting_back{ false }
{}

void klib::Timer::move(int p_delta_ms) {
	cur_cnt += p_delta_ms;
	if (cur_cnt > ms_per_frame) {
		cur_cnt = 0;

		if (is_counting_back)
			--cur_frame;
		else
			++cur_frame;

		if (cur_frame >= max_no) {
			if (backward) {
				is_counting_back = true;
				cur_frame = max_no;
			}
			else
				cur_frame = 0;
		}
		else if (cur_frame < 0) {
			is_counting_back = false;
			cur_frame = 0;
		}
	}
}

int klib::Timer::get_frame(void) const {
	return this->cur_frame;
}

void klib::Timer::reset(void) {
	this->cur_cnt = 0;
	this->cur_frame = 0;
}
