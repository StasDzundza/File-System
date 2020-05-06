#pragma once

namespace filesystem::io {
	class IOSystemInterface {
	protected:
		void _close_fs();
		bool _file_exists(const char *path) const;
		void _init(const char* _system_state_path);
	public:
		/* Copies the logical disk block i into memory starting from the
		 * address specified by the pointer p. The number of characters copied
		 * equals to the logical block length. */
		virtual void read_block(int i, char *p) = 0;

		/* Copies the number of characters corresponding to the logical block length,
		 * starting from the memory address specified by the pointer p, into
		 * the logical disk block i. */
		virtual void write_block(int i, char *p) = 0;

		virtual void save_system_state() = 0;
		virtual void save_system_state(const char* filename) = 0;
		virtual void restore_system_state() = 0;
	protected:
		const char* _system_state_path = nullptr;
	};
} // namespace filesystem::io
