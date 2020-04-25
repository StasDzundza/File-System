#ifndef INPUT_OUTPUT_SYSTEM_H
#define INPUT_OUTPUT_SYSTEM_H

#include "../fs_config.h"
namespace filesystem::io {
	using namespace config;
	class IOSystem {
	private:
		bool file_exists(const char *path) const;

	public:
		IOSystem();
		void init(const char *system_state_path);
		~IOSystem();

		// This copies the logical block ldisk[i] into main memory starting at the
		// location specified by the pointer p. The number of characters copied
		// corresponds to the block length, B.
		void read_block(int block_idx, char *copy_to_ptr);

		// This copies the number of characters corresponding to the block length, B,
		// from main memory starting at the location specified by the pointer p, into
		// the logical block ldisk[i].
		void write_block(int block_idx, char *copy_from_ptr);

		// saves current disk state to the file with path specified in the
		// storage_path
		void save_system_state();

		// restores current disk state from the file with path specified in the
		// storage_path
		void restore_system_state();

	protected:
		int _blocks_num;
		int _block_len;
		char _ldisk[DISC_BLOCKS_NUM][BLOCK_SIZE];

		const char *_system_state_path;
	};

} // namespace filesystem::io

#endif