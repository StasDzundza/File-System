#pragma once

#include "IOSystem.h"
#include "io_config.h"

namespace filesystem::io {
	using namespace io::config;
	class LBASystem : public IOSystemInterface {
	private:
		bool _check_config();
	public:
		LBASystem();
		void init(int blocks_num, int block_len, const char *system_state_path);
		~LBASystem();

		void read_block(int block_idx, char *copy_to_ptr) override;
		void write_block(int block_idx, char *copy_from_ptr) override;

		void save_system_state() override;
		void save_system_state(const char* filename) override;
		void restore_system_state() override;

	protected:
		int _blocks_num;
		int _block_len;
		char _ldisk[MAX_BLOCKS_NUM][MAX_BLOCK_SIZE];
	};
}