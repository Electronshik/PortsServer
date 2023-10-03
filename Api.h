#pragma once
#include "main.h"
#include "httplib.h"

using namespace httplib;

namespace Api
{
	void GetPortsList(const Request& req, Response& res);
	void OpenPort(const Request& req, Response& res);
	void ClosePort(const Request& req, Response& res);
	void SendToPort(const Request& req, Response& res);
	void ReadFromPort(const Request& req, Response& res);
	void GetTestData(const Request& req, Response& res);
}
