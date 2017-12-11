#include "Converter.h"

std::string CoilCl::Conv::Cast::PrintTag(Cast::Tag tag)
{
	switch (tag) {
	case Tag::INTEGRAL_CAST:
		return "integral cast";
	case Tag::ARRAY_CAST:
		return "array cast";
	case Tag::LTOR_CAST:
		return "ltor cast";
	case Tag::FUNCTION_CAST:
		return "function cast";
	}

	return "<unknown>";
}
