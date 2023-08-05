module;

#include <format>
#include <string_view>
#include <cstdio>

export module utils;

export template<typename... Args>
void print(const std::string_view fmt_str, Args&&... args)
{
	auto fmt_args{ std::make_format_args(args...) };
	std::string outstr{ std::vformat(fmt_str, fmt_args) };
	std::fputs(outstr.c_str(), stdout);
}

export template<typename... Args>
void print_nl(const std::string_view fmt_str, Args&&... args)
{
	auto fmt_args{ std::make_format_args(args...) };
	std::string outstr{ std::vformat(fmt_str, fmt_args) };
	outstr.append("\n");
	std::fputs(outstr.c_str(), stdout);
}
