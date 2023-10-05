#include "View.h"

#include <json.hpp>
#include <inja.hpp>

#include <fstream>
#include <iostream>

import Utils;

using namespace inja;

// static Environment InjaEnv {"../../html/"};
static std::unique_ptr<Environment> InjaEnv;

View::View()
{
	InjaEnv = make_unique<Environment>(HtmlGlobalPath + "html/");
}

View::~View()
{
}

auto View::GetIndex(std::vector<Command> &commands) -> std::string
{
	// std::ifstream index("../html/index.html");
	// std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());

	json data;
	data["error_code_ok"] = ErrorString[ErrorCode::Ok];
	data["error_code_error"] = ErrorString[ErrorCode::Error];

	data["port"] = "{{ port }}";
	data["value"] = "{{ value }}";
	data["label"] = "{{ label }}";
	data["cmd"] = "{{ cmd }}";
	data["name"] = "{{ name }}";
	data["conf"] = "{{ conf }}";

	std::vector<std::string> outjson_vec;
	std::transform(commands.begin(), commands.end(), std::back_inserter(outjson_vec), [](Command& cmd) {
		json command_json;
		command_json["cmd"] = cmd.Cmd;
		command_json["name"] = cmd.Name;
		return command_json.dump();
	});

	std::string outjson_str{"\n"};
	outjson_str.append(join_with_sep(outjson_vec.begin(), outjson_vec.end(), ",\n"));

	data["commands_array"] = outjson_str;

	return InjaEnv->render_file("index.html", data);
}
