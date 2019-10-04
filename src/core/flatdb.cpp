#include "spjalla/core/flatdb.h"
#include "spjalla/core/util.h"

namespace spjalla {

// Protected static methods


	std::filesystem::path flatdb::get_db_path(const std::string &dbname, const std::string &dirname) {
		if (!dirname.empty() && dirname.front() == '/')
			return std::filesystem::path(dirname) / dbname;
		return util::get_home() / dirname / dbname;
	}

	std::pair<std::string, std::string> flatdb::parse_kv_pair(const std::string &str) {
		if (str.empty())
			throw std::invalid_argument("Can't parse empty string as key-value pair");

		const size_t equals = str.find('=');

		if (equals == std::string::npos)
			throw std::invalid_argument("No equals sign found in key-value pair");

		if (equals == 0 || equals == str.find_first_not_of(" "))
			throw std::invalid_argument("Empty key in key-value pair");

		std::string key = str.substr(0, equals);
		util::trim(key);

		for (char ch: key) {
			if (!std::isalnum(ch) && ch != '.' && ch != '_')
				throw std::invalid_argument("Key isn't alphanumeric, '.' or '_' in key-value pair");
		}

		return {key, util::trim(str.substr(equals + 1))};
	}

	std::string flatdb::parse_string(std::string value) {
		util::trim(value);
		const size_t vlength = value.length();

		// Special case: an empty value represents an empty string, same as a pair of double quotes.
		if (vlength == 0)
			return "";

		if (vlength < 2)
			throw std::invalid_argument("Invalid length of string value");

		if (value.front() != '"' || value.back() != '"')
			throw std::invalid_argument("Invalid quote placement in string value");

		return util::unescape(value.substr(1, vlength - 2));
	}

	std::pair<std::string, std::string> flatdb::parse_string_line(const std::string &str) {
		std::string key, value;
		std::tie(key, value) = parse_kv_pair(str);

		return {key, parse_string(value)};
	}


// Protected instance methods


	void flatdb::write_db() {
		std::ofstream out {filepath};
		out << std::string(*this);
		out.close();
	}

	void flatdb::read_db(bool apply, bool clear) {
		if (clear)
			clear_all();

		std::ifstream in {filepath};
		std::string line;
		while (std::getline(in, line)) {
			apply_line(line);
		}

		if (apply)
			apply_all();
	}


// Public instance methods


	void flatdb::set_path(const std::string &dbname, bool apply, const std::string &dirname) {
		ensure_db(dbname, dirname);
		filepath = get_db_path(dbname, dirname);
		read_db(apply);
	}

	void flatdb::read_if_empty(const std::string &dbname, bool apply, const std::string &dirname) {
		if (filepath.empty())
			set_path(dbname, apply, dirname);
		else if (empty())
			read_db(apply);
	}


// Public static methods


	bool flatdb::ensure_dir(const std::string &name) {
		if (name.empty())
			throw std::invalid_argument("Directory path is empty");
		std::filesystem::path config_path = name.front() == '/'? std::filesystem::path(name) : util::get_home() / name;
		if (!std::filesystem::exists(config_path)) {
			std::filesystem::create_directory(config_path);
			return true;
		}

		return false;
	}

	bool flatdb::ensure_db(const std::string &dbname, const std::string &dirname) {
		ensure_dir(dirname);
		std::filesystem::path db_path = get_db_path(dbname, dirname);

		bool created = false;
		if (!std::filesystem::exists(db_path)) {
			std::ofstream(db_path).close();
			created = true;
		}

		return created;
	}
}
