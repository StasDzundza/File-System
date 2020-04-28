#pragma once

#include <tuple>

#include "IOSystem.h"
#include "io_config.h"

namespace filesystem::io {
	using namespace io::config;
	class CHSSystem : public IOSystemInterface {
	private:
		typedef std::tuple<int, int, int> chs_idx;
		bool _check_config();
		chs_idx lba_to_chs(int logical_block_idx);
	public:
		CHSSystem();
		void init(int c, int t, int s, int b, const char *system_state_path);
		~CHSSystem();

		void read_block(int block_idx, char *copy_to_ptr) override;
		void write_block(int block_idx, char *copy_from_ptr) override;

		void save_system_state() override;
		void restore_system_state() override;

	private:
		int c, t, s, b;
		char _ldisk[C_MAX][T_MAX][S_MAX][B_MAX];
	};
}