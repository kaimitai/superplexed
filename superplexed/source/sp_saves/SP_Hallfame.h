#ifndef SP_HALLFAME_H
#define SP_HALLFAME_H

#include <string>
#include <vector>

using byte = unsigned char;

class SP_Hallfame {

	struct HF_Entry {
		std::string m_name;
		byte m_hrs, m_mins, m_secs;
		HF_Entry(void);
		HF_Entry(const std::string& p_name,
			byte p_hrs, byte p_mins, byte p_secs);
	};

	std::vector<HF_Entry> m_entries;

public:
	SP_Hallfame(void);
	SP_Hallfame(const std::vector<byte>& p_bytes);
	std::vector<byte> to_bytes(void) const;

	// getters
	std::string get_name(int p_entry_no) const;
	byte get_hrs(int p_entry_no) const;
	byte get_mins(int p_entry_no) const;
	byte get_secs(int p_entry_no) const;

	// setters
	void set_name(int p_entry_no, const std::string& p_name);
	void set_hrs(int p_entry_no, byte p_hrs);
	void set_mins(int p_entry_no, byte p_mins);
	void set_secs(int p_entry_no, byte p_secs);
};

#endif
