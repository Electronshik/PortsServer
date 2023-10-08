#include "View.h"

#include <json.hpp>
#include <inja.hpp>

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

auto View::GetIndex() const -> std::string
{
	// std::ifstream index("../html/index.html");
	// std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());

	json data;
	data["inja_error_code_ok"] = ErrorString[ErrorCode::Ok];
	data["inja_error_code_error"] = ErrorString[ErrorCode::Error];

	data["port"] = "{{ port }}";
	data["value"] = "{{ value }}";
	data["label"] = "{{ label }}";
	data["cmd"] = "{{ cmd }}";
	data["name"] = "{{ name }}";
	data["conf"] = "{{ conf }}";

	return InjaEnv->render_file("index.html", data);
}
