#pragma once
#include "main.h"

#if defined(WIN64) || defined(_WIN64) || defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__)
	#define SERIAL_PORT_WINDOWS
	#include <windows.h>
#else
	#include <sys/ioctl.h>
	#include <termios.h>
#endif

class SerialPort
{
	public:
		enum Speed {
			s_9600,
			s_19200,
			s_115200,
		};

		enum Databits {
			b5,
			b6,
			b7,
			b8,
		};

		enum Parity {
			None,
			Odd,
			Even,
		};

		enum Stopbits {
			b1,
			b2
		};

		enum Flowctrl {
			No,
			Yes,
		};

		struct Config
		{
			SerialPort::Speed Speed = SerialPort::Speed::s_115200;
			SerialPort::Databits Databits = SerialPort::Databits::b8;
			SerialPort::Parity Parity = SerialPort::Parity::None;
			SerialPort::Stopbits Stopbits = SerialPort::Stopbits::b1;
			SerialPort::Flowctrl Flowctrl = SerialPort::Flowctrl::No;
		};

		static auto GetPortsList() -> std::vector<std::string>;
		SerialPort(std::string &name, Config &serial_config);
		~SerialPort();
		auto GetName() -> std::string;
		void Write(char* buff, int len);
		auto Read(char* buff) -> unsigned int;

	private:
		std::string name;
#if defined(SERIAL_PORT_WINDOWS)
		HANDLE port;
		DCB config;
#else
		int	f_descr;
#endif
};
