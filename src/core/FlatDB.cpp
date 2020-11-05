#include "spjalla/core/FlatDB.h"
#include "spjalla/core/Util.h"

namespace Spjalla {

// Protected static methods


	std::filesystem::path FlatDB::getDBPath(const std::string &dbname, const std::string &dirname) {
		if (!dirname.empty() && dirname.front() == '/')
			return std::filesystem::path(dirname) / dbname;
		return Util::getHome() / dirname / dbname;
	}

	std::pair<std::string, std::string> FlatDB::parseKVPair(const std::string &str) {
		if (str.empty())
			throw std::invalid_argument("Can't parse empty string as key-value pair");

		const size_t equals = str.find('=');

		if (equals == std::string::npos)
			throw std::invalid_argument("No equals sign found in key-value pair");

		if (equals == 0 || equals == str.find_first_not_of(" "))
			throw std::invalid_argument("Empty key in key-value pair");

		std::string key = str.substr(0, equals);
		formicine::util::trim(key);

		for (char ch: key) {
			if (!std::isalnum(ch) && ch != '.' && ch != '_')
				throw std::invalid_argument("Key isn't alphanumeric, '.' or '_' in key-value pair");
		}

		return {key, formicine::util::trim(str.substr(equals + 1))};
	}

	std::string FlatDB::parseString(std::string value) {
		formicine::util::trim(value);
		const size_t vlength = value.length();

		// Special case: an empty value represents an empty string, same as a pair of double quotes.
		if (vlength == 0)
			return "";

		if (vlength < 2)
			throw std::invalid_argument("Invalid length of string value");

		if (value.front() != '"' || value.back() != '"')
			throw std::invalid_argument("Invalid quote placement in string value");

		return Util::unescape(value.substr(1, vlength - 2));
	}

	std::pair<std::string, std::string> FlatDB::parseStringLine(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parseKVPair(str);

		return {key, parseString(value)};
	}


// Protected instance methods


	void FlatDB::writeDB() {
		std::ofstream out {filepath};
		out << std::string(*this);
		out.close();
	}

	void FlatDB::readDB(bool apply, bool clear) {
		if (clear)
			clearAll();

		std::ifstream in {filepath};
		std::string line;
		while (std::getline(in, line)) {
			applyLine(line);
		}

		if (apply)
			applyAll();
	}


// Public instance methods


	void FlatDB::setPath(const std::string &dbname, bool apply, const std::string &dirname) {
		ensureDB(dbname, dirname);
		filepath = getDBPath(dbname, dirname);
		readDB(apply);
	}

	void FlatDB::readIfEmpty(const std::string &dbname, bool apply, const std::string &dirname) {
		if (filepath.empty())
			setPath(dbname, apply, dirname);
		else if (empty())
			readDB(apply);
	}


// Public static methods


	bool FlatDB::ensureDirectory(const std::string &name) {
		if (name.empty())
			throw std::invalid_argument("Directory path is empty");
		std::filesystem::path config_path = name.front() == '/'? std::filesystem::path(name) : Util::getHome() / name;
		if (!std::filesystem::exists(config_path)) {
			std::filesystem::create_directory(config_path);
			return true;
		}

		return false;
	}

	bool FlatDB::ensureDB(const std::string &dbname, const std::string &dirname) {
		ensureDirectory(dirname);
		std::filesystem::path db_path = getDBPath(dbname, dirname);

		bool created = false;
		if (!std::filesystem::exists(db_path)) {
			std::ofstream(db_path).close();
			created = true;
		}

		return created;
	}
}
