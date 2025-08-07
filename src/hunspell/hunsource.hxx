#pragma once
#include <istream>
#include <variant>
#include <memory>
#include "csutil.hxx"

using HunStreamPtr = std::shared_ptr<std::istream>;

class HunSource {
	using Impl = std::variant<HunStreamPtr, char const*>;
	std::shared_ptr<Impl> pImpl;
public:
	template<
		typename T,
		typename = std::enable_if_t<std::is_constructible_v<Impl, T&&>>
	>
	HunSource(T && v)
		: pImpl(std::make_shared<Impl>(std::forward<T>(v))) {
			
	}
	template<typename T, typename ...Args>
	static HunSource Stream(Args &&...args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
	auto path() const {
		auto* pPath = get_if<char const*>(pImpl.get());
		return pPath ? *pPath : nullptr;
	}
	std::istream const* stream() const& {
		auto* ppStream = get_if<HunStreamPtr>(pImpl.get());
		return ppStream ? ppStream->get() : nullptr;
	}
	HunStreamPtr stream() && {
		if (auto* ppStream = get_if<HunStreamPtr>(pImpl.get())) {
			assert(ppStream->use_count() == 1); // assume the stream is not used anywhere else
			auto pStream = *ppStream;
			pStream->clear(); // seekg() will not work if the failbit is set
			pStream->seekg(0); // reset stream position
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
