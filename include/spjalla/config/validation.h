#ifndef SPJALLA_CONFIG_VALIDATION_H_
#define SPJALLA_CONFIG_VALIDATION_H_

#include <stdexcept>

namespace Spjalla::Config {
	enum class ValidationResult {Valid, BadType, BadValue};

	struct ValidationFailure: public std::exception {
		ValidationResult result;
		ValidationFailure(ValidationResult result_): result(result_) {}

		const char * what() const noexcept {
			switch (result) {
				case ValidationResult::Valid:  return "Configuration validated successfully but threw anyway, somehow?";
				case ValidationResult::BadType:  return "Invalid value type";
				case ValidationResult::BadValue: return "Invalid value";
				default: return "???";
			}
		}
	};
}

#endif
