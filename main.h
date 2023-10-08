#pragma once
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <unordered_set>
#include <tuple>

import Setting;

extern std::string HtmlGlobalPath;

enum class ErrorCode
{
	Ok,
	Error
};

extern std::map<ErrorCode, std::string> ErrorString;

template <typename T = std::string, class V = std::vector<T>>
struct SerialPortSettings
{
	Setting<T, V> Speed;
	Setting<T, V> Databits;
	Setting<T, V> Parity;
	Setting<T, V> Stopbits;
	Setting<T, V> Flowcontrol;
	// std::string Format = "ASCII";

	std::unordered_map<std::string, Setting<T, V>*> AllParamsList = {
		{"speed", &this->Speed},
		{"databits", &this->Databits},
		{"parity", &this->Parity},
		{"stopbits", &this->Stopbits},
		{"flowcontrol", &this->Flowcontrol}
	};
	bool ContainsValue(std::string name, T value)
	{
		if (this->AllParamsList[name]->Contains(value))
			return true;
		return false;
	}
};

extern SerialPortSettings<std::string, std::vector<std::string>> SerialPortAllSettings;

struct SerialPortConfig
{
	std::string Speed;
	std::string Databits;
	std::string Parity;
	std::string Stopbits;
	std::string Flowcontrol;
	std::string Format = "ASCII";

	struct HashFunction
	{
		size_t operator()(const std::tuple<const char*, std::string*> &x) const
		{
			return std::hash<const char*>{}(get<0>(x));
		}
	};
	std::unordered_set<std::tuple<const char*, std::string*>, HashFunction> ParamsList = {
		{"speed", &this->Speed},
		{"databits", &this->Databits},
		{"parity", &this->Parity},
		{"stopbits", &this->Stopbits},
		{"flowcontrol", &this->Flowcontrol}
	};

	SerialPortConfig() :
		Speed(SerialPortAllSettings.Speed.GetDefaultValue()),
		Databits(SerialPortAllSettings.Databits.GetDefaultValue()),
		Parity(SerialPortAllSettings.Parity.GetDefaultValue()),
		Stopbits(SerialPortAllSettings.Stopbits.GetDefaultValue()),
		Flowcontrol(SerialPortAllSettings.Flowcontrol.GetDefaultValue()) {}
};

struct Command
{
	std::string Name;
	std::string Cmd;
	Command() {}
	Command(Command&& moved) = default;
};

auto ParsePortConfig(const std::string &str, SerialPortConfig *config) -> bool;
auto ParseGetPostParam(const std::string &str, const std::string &name) -> std::optional<std::string>;
