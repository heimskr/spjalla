#include <sstream>
#include <stdexcept>

#include "spjalla/core/Aliases.h"
#include "spjalla/core/Util.h"

namespace Spjalla {
	void Aliases::addAlias(const std::string &key, const std::string &expansion) {
		if (expansion.empty())
			throw std::invalid_argument("Alias expansion is empty");

		db.erase(key);
		db.insert({formicine::util::lower(key), expansion});
	}

	bool Aliases::hasAlias(const std::string &key) {
		return db.count(key) != 0;
	}

	InputLine & Aliases::expand(InputLine &line) {
		std::string lcommand = line.command;
		if (!line.isCommand() || !hasAlias(lcommand))
			return line;

		const std::string &expansion = db.find(lcommand)->second;
		std::vector<std::string> split = Util::split(expansion, " ", false);
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

	std::pair<std::string, std::string> Aliases::applyLine(const std::string &line) {
		if (!line.empty() && line.front() == '#')
			return {"", ""};

		std::string key, expansion;
		std::tie(key, expansion) = parseKVPair(line);
		if (hasAlias(key))
			remove(key);
		insert(key, expansion);
		return {key, db.at(key)};
	}

	bool Aliases::insert(const std::string &key, const std::string &expansion, bool save) {
		bool overwritten = false;
		if (hasAlias(key)) {
			remove(key, false);
			overwritten = true;
		}

		db.insert({key, expansion});

		if (save)
			writeDB();

		return overwritten;
	}

	bool Aliases::remove(const std::string &key, bool save) {
		if (!hasAlias(key))
			return false;

		db.erase(key);

		if (save)
			writeDB();

		return true;
	}

	std::string Aliases::get(const std::string &key) {
		return db.at(key);
	}

	Aliases::operator std::string() const {
		std::ostringstream out;
		for (const auto &pair: db)
			out << pair.first << "=" << Util::escape(pair.second) << "\n";
		return out.str();
	}
}
