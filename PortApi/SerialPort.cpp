#include "SerialPort.h"

auto SerialPort::GetName() -> std::string
{
	return this->name;
}

#if defined(SERIAL_PORT_WINDOWS)

auto SerialPort::GetPortsList() -> std::vector<std::string>
{
	COMMCONFIG CommConfig;
	DWORD size;

	std::vector<std::string> result;

	for (int i = 0; i <= 255; i++)
	{
		size = sizeof CommConfig;
		std::string port_name = "COM" + std::to_string(i);
		if(GetDefaultCommConfig((LPCSTR)port_name.c_str(), &CommConfig, &size) || size > sizeof CommConfig)
			result.push_back(std::move(port_name));
	}

	return result;
}

SerialPort::SerialPort(std::string &name, Config &serial_config)
{
	this->name = name;
	std::string win_name = "\\\\.\\" + name;

	this->port = CreateFile((LPCSTR)win_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (this->port == INVALID_HANDLE_VALUE)
	{
		DWORD lastError = GetLastError();
		return;
	}

	SetupComm(this->port, 256, 256); 

	this->config.fOutxCtsFlow = FALSE;
	this->config.fOutxDsrFlow = FALSE;
	this->config.fOutX = FALSE;
	this->config.fInX = FALSE;
	this->config.fNull = 0;
	this->config.fAbortOnError = 0;

	switch (serial_config.Speed)
	{
		case Speed::s_9600:
			this->config.BaudRate = 9600;
			break;

		case Speed::s_19200:
			this->config.BaudRate = 19200;
			break;

		default:
			this->config.BaudRate = 115200;
			break;
	}

	if (serial_config.Databits == Databits::b7)
		this->config.ByteSize = 7U;
	else
		this->config.ByteSize = 8U;

	if (serial_config.Parity == Parity::Odd)
		this->config.Parity = ODDPARITY;
	else if (serial_config.Parity == Parity::Even)
		this->config.Parity = EVENPARITY;
	else
		this->config.Parity = NOPARITY;

	if (serial_config.Stopbits == Stopbits::b1)
		this->config.StopBits = ONESTOPBIT;
	else
		this->config.StopBits = TWOSTOPBITS;

	COMMTIMEOUTS timeouts = { MAXDWORD, 0, 20, 0, 0 };
	if (!SetCommTimeouts(this->port, &timeouts) || !SetCommState(this->port, &this->config))
	{
		CloseHandle(this->port);
	}

	PurgeComm(this->port, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

SerialPort::~SerialPort()
{
	CloseHandle(this->port);
}

void SerialPort::Write(char* buff, int len)
{
	DWORD written_len;
	WriteFile(this->port, buff, len, &written_len, NULL);
}

auto SerialPort::Read(char* buff) -> unsigned int
{
	DWORD readed_len = 0;
	unsigned int len = 0;

	do {
		ReadFile(this->port, (buff + len), 1, &readed_len, NULL);
		len += readed_len;

	} while (readed_len > 0);

	return len;	
}

#else

auto SerialPort::GetPortsList() -> std::vector<std::string>
{

}

SerialPort::SerialPort(std::string &name, Config &serial_config)
{

}

SerialPort::~SerialPort()
{		
	if (this->f_descr > 0)
	{
		close(this->f_descr);
	}
}

void SerialPort::Write(char* buff, int len)
{
	write(this->f_descr, pos, len);
}

auto SerialPort::Read(char* buff) -> unsigned int
{
	ssize_t res = 0;
	unsigned int len = 0;

	do
	{			
		res = read(f_descr, (buff + len), 1);			
		if (res > 0)
			len++;

	} while (res > 0);

	return len;
}

#endif
