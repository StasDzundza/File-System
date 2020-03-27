#ifndef INPUT_OUTPUT_SYSTEM_H
#define INPUT_OUTPUT_SYSTEM_H

namespace filesystem::io {

	class IOSystemInterface {
	public:
		virtual void read_block(int i, char *p) = 0;
		virtual void write_block(int i, char *p) = 0;
	};

	class IOSystem : public IOSystemInterface {
	private:
		bool file_exists(const char *path) const;
		char **reserve_disk_space(int row_size, int col_size);
		void free_disk_space(char **disk_ptr, int row_size, int col_size);

	public:
		IOSystem(const char *system_state_path);
		IOSystem(int blocks_amount, int block_size, const char *system_state_path);
		~IOSystem();

		// This copies the logical block ldisk[i] into main memory starting at the
		// location specified by the pointer p. The number of characters copied
		// corresponds to the block length, B.
		void read_block(int block_idx, char *copy_to_ptr) override;

		// This copies the number of characters corresponding to the block length, B,
		// from main memory starting at the location specified by the pointer p, into
		// the logical block ldisk[i].
		void write_block(int block_idx, char *copy_from_ptr) override;

		// saves current disk state to the file with path specified in the
		// storage_path
		void save_system_state();

		// restores current disk state from the file with path specified in the
		// storage_path
		void restore_system_state();

	private:
		int _blocks_num;
		int _block_len;
		char **_ldisk;

		const char *_system_state_path;
	};

} // namespace filesystem::io

#endif