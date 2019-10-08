#ifndef SPJALLA_UTIL_BACKWARD_READER_H_
#define SPJALLA_UTIL_BACKWARD_READER_H_

#include <fstream>
#include <string>
#include <vector>

namespace spjalla::util {
	class backward_reader {
		private:
			std::fstream stream;

			const ssize_t chunk_size;
			std::string buffer;
			char *chunk;

			/** Set to true once the very beginning of the file has been read. Continuing past that point would
			 *  repeatedly return the same input in readline. */
			bool done = false;

			void remove_last_newline(size_t &nlpos);

		public:
			backward_reader(std::fstream &&stream_, ssize_t chunk_size_ = 64);
			backward_reader(const std::string &filename, ssize_t chunk_size_ = 64,
			std::ios_base::openmode mode = std::ios_base::in):
				backward_reader(std::fstream(filename, mode), chunk_size_) {}

			~backward_reader();

			/** Reads a single line. Returns true if a line was properly read, or false if there's nothing left. */
			bool readline(std::string &);

			/** Reads multiple lines. Returns the number of lines read. */
			size_t readlines(std::vector<std::string> &, size_t);

			/** Resets the position and buffers. */
			void reset();
	};
}

#endif
