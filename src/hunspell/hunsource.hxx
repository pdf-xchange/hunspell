#pragma once
#include <istream>
#include <variant>
#include <memory>
#include "csutil.hxx"

using HunStreamPtr = std::shared_ptr<std::istream>;
//using HunSource = std::variant<HunStreamPtr, char const*>;

class HunSource : std::variant<HunStreamPtr, char const*> {
public:
	using std::variant<HunStreamPtr, char const*>::variant;
	auto path() const {
		auto *pPath = get_if<char const*>(this);
		return pPath ? *pPath : nullptr;
	}
	std::istream const * stream() const & {
		auto *ppStream = get_if<HunStreamPtr>(this);
		return ppStream ? ppStream->get() : nullptr;
	}
	HunStreamPtr stream() && {
		if (auto* ppStream = get_if<HunStreamPtr>(this)) {
			//assert(ppStream->use_count() == 1); //TODO Assert that we are the only owner of the stream
			auto pStream = std::move(*ppStream);
			pStream->clear();
			pStream->seekg(0, std::ios_base::beg); // reset stream position
			assert(pStream->tellg() == 0);
			return pStream;
		}
		return nullptr;
	}
};

inline
std::unique_ptr<std::istream> myopen(const char* path, std::ios_base::openmode mode) {
	if (auto pstream = std::make_unique<std::ifstream>()) {
		myopen(*pstream, path, mode);
		if (pstream->is_open()) {
			return pstream;
		}
	}
	return {};
}
