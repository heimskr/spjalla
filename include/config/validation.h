#ifndef SPJALLA_CONFIG_VALIDATION_H_
#define SPJALLA_CONFIG_VALIDATION_H_

#include <stdexcept>

namespace spjalla::config {
	enum class validation_result {valid, bad_type, bad_value};

	struct validation_failure: public std::exception {
		validation_result result;
		validation_failure(validation_result result_): result(result_) {}

		const char * what() const noexcept {
			switch (result) {
				case validation_result::valid: return "Configuration validated successfully but threw anyway, somehow?";
				case validation_result::bad_type: return "Invalid value type";
				case validation_result::bad_value: return "Invalid value";
				default: return "???";
			}
		}
	};
}

#endif
