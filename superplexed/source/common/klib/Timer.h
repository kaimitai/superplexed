#ifndef KLIB_TIMER_H
#define KLIB_TIMER_H

namespace klib {

	class Timer {
		int max_no, ms_per_frame, cur_frame, cur_cnt;
		bool backward;
		bool is_counting_back;

	public:
		Timer(int p_max_no, int p_ms_per_frame, bool p_backward = false);
		void move(int p_delta_ms);
		void reset(void);
		int get_frame(void) const;
	};

}

#endif

