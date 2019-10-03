#include <stdexcept>

#include "spjalla/core/aliases.h"
#include "spjalla/core/util.h"
// #include "lib/formicine/futil.h"

namespace spjalla {
	void aliases::add_alias(const std::string &key, const std::string &expansion) {
		if (expansion.empty())
			throw std::invalid_argument("Alias expansion is empty");

		map.erase(key);
		map.insert({formicine::util::lower(key), expansion});
	}

	bool aliases::has_alias(const std::string &key) {
		return map.count(key) != 0;
	}

	input_line & aliases::expand(input_line &line) {
		std::string lcommand = util::lower(line.command);
		if (!line.is_command() || !has_alias(lcommand))
			return line;

		const std::string &expansion = map.find(lcommand)->second;
		std::vector<std::string> split = util::split(expansion, " ", false);
		line.command = split[0];

		if (split.size() == 1)
			return line;

		line.args.insert(line.args.begin(), split.begin() + 1, split.end());

		std::string body_start;
		for (const std::string &piece: split)
			body_start += piece + " ";
		line.body.insert(0, body_start);

		return line;
	}
}
