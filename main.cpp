#include "httplib.h"
#include "sqlite3.h"
#include <iostream>

int main(void)
{
	using namespace httplib;

	Server server;

	server.Get("/", [](const Request& req, Response& res)
	{
		std::ifstream index("../html/index.html");
		std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		std::cout << req.path << std::endl;

		res.set_content(body, "text/html");
	});

	server.Get("/html/bootstrap-5.3.0-dist/css/bootstrap.min.css", [](const Request& req, Response& res)
	{
		std::ifstream index("../html/bootstrap-5.3.0-dist/css/bootstrap.min.css");
		std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		std::cout << req.path << std::endl;

		res.set_content(body, "text/css");
	});

	server.Get("/html/bootstrap-5.3.0-dist/js/bootstrap.bundle.min.js", [](const Request& req, Response& res)
	{
		std::ifstream index("../html/bootstrap-5.3.0-dist/js/bootstrap.bundle.min.js");
		std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		std::cout << req.path << std::endl;

		res.set_content(body, "text/plain");
	});

	server.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
		auto numbers = req.matches[1];
		res.set_content(numbers, "text/plain");
	});

	server.Get("/body-header-param", [](const Request& req, Response& res) {
		if (req.has_header("Content-Length")) {
			auto val = req.get_header_value("Content-Length");
		}
		if (req.has_param("key")) {
			auto val = req.get_param_value("key");
		}
		res.set_content(req.body, "text/plain");
	});

	server.Get("/stop", [&](const Request& req, Response& res) {
		server.stop();
	});


   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;

   rc = sqlite3_open("test.db", &db);

   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }
   sqlite3_close(db);


	server.listen("localhost", 80);
}
