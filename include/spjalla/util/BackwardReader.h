#ifndef SPJALLA_UTIL_BACKWARDREADER_H_
#define SPJALLA_UTIL_BACKWARDREADER_H_

#include <fstream>
#include <string>
#include <vector>

namespace Spjalla::Util {
	class BackwardReader {
		private:
			std::fstream stream;

			const ssize_t chunk_size;
			std::string buffer;
			char *chunk;

			/** Set to true once the very beginning of the file has been read. Continuing past that point would
			 *  repeatedly return the same input in readline. */
			bool done = false;

			void removeLastNewline(size_t &nlpos);

		public:
			BackwardReader(std::fstream &&stream_, ssize_t chunk_size_ = 64);

			BackwardReader(const std::string &filename, ssize_t chunk_size_ = 64,
			std::ios_base::openmode mode = std::ios_base::in):
				BackwardReader(std::fstream(filename, mode), chunk_size_) {}

			~BackwardReader();

			/** Reads a single line. Returns true if a line was properly read, or false if there's nothing left. */
			bool readline(std::string &);

			/** Reads multiple lines. Returns the number of lines read. */
			size_t readlines(std::vector<std::string> &, size_t);

			/** Resets the position and buffers. */
			void reset();
	};
}

#endif
