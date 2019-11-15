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
		std::string lcommand = line.command;
		if (!line.is_command() || !has_alias(lcommand))
			return line;

		const std::string &expansion = db.find(lcommand)->second;
		std::vector<std::string> split = util::split(expansion, " ", false);
		std::string &first = split[0];

		// For non-command expansions, the entire expansion is inserted into the body. For command expansions,
		// it's necessary to skip the first item because the command isn't part of the body.
		size_t body_offset = 0;

		if (first.front() == '/') {
			// If the expansion begins with a slash, it's a command. Replace the aliased line's command with the
			// expansion's command.
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
		if (line.body.empty() && !body_start.empty() && body_start.back() == ' ')
			body_start.pop_back();
		line.body.insert(0, body_start);

		return line;
	}

	std::pair<std::string, std::string> aliases::apply_line(const std::string &line) {
		if (!line.empty() && line.front() == '#')
			return {"", ""};

		std::string key, expansion;
		std::tie(key, expansion) = parse_kv_pair(line);
		if (has_alias(key))
			remove(key);
		insert(key, expansion);
		return {key, db.at(key)};
	}

	bool aliases::insert(const std::string &key, const std::string &expansion, bool save) {
		bool overwritten = false;
		if (has_alias(key)) {
			remove(key, false);
			overwritten = true;
		}

		db.insert({key, expansion});

		if (save)
			write_db();

		return overwritten;
	}

	bool aliases::remove(const std::string &key, bool save) {
		if (!has_alias(key))
			return false;

		db.erase(key);

		if (save)
			write_db();

		return true;
	}

	std::string aliases::get(const std::string &key) {
		return db.at(key);
	}

	aliases::operator std::string() const {
		std::ostringstream out;
		for (const auto &pair: db)
			out << pair.first << "=" << util::escape(pair.second) << "\n";
		return out.str();
	}
}
