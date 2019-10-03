#include <sstream>
#include <stdexcept>

#include "spjalla/core/aliases.h"
#include "spjalla/core/util.h"

namespace spjalla {
	void aliases::add_alias(const std::string &key, const std::string &expansion) {
		if (expansion.empty())
			throw std::invalid_argument("Alias expansion is empty");

		db.erase(key);
		db.insert({formicine::util::lower(key), expansion});
	}

	bool aliases::has_alias(const std::string &key) {
		return db.count(key) != 0;
	}

	input_line & aliases::expand(input_line &line) {
		std::string lcommand = util::lower(line.command);
		if (!line.is_command() || !has_alias(lcommand))
			return line;

		const std::string &expansion = db.find(lcommand)->second;
		std::vector<std::string> split = util::split(expansion, " ", false);
		std::string &first = split[0];

		size_t body_offset = 0;
		if (first.front() == '/') {
			first.erase(0, 1);
			line.command = split[0];
			body_offset = 1;

			if (split.size() == 1)
				return line;
		} else {
			line.command.clear();
		}

		line.args.insert(line.args.begin(), split.begin() + body_offset, split.end());

		std::string body_start;
		for (auto iter = split.begin() + body_offset, end = split.end(); iter != end; ++iter)
			body_start += *iter + " ";
		line.body.insert(0, body_start);

		return line;
	}

	void aliases::apply_line(const std::string &line) {

	}

	void aliases::apply_all() {
		
	}

	aliases::operator std::string() const {
		std::ostringstream out;
		for (const auto &pair: db)
			out << pair.first << "=\"" << util::escape(pair.second) << "\"\n";
		return out.str();
	}
}
