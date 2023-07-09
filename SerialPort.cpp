#include "SerialPort.h"

std::vector<std::string> SerialPort::GetPortsList()
{
	COMMCONFIG CommConfig;
	DWORD size;

	std::vector<std::string> result;

	for (int i = 0; i <= 255; i++)
	{
		size = sizeof CommConfig;
		std::string port_name = "COM" + std::to_string(i);
		if(GetDefaultCommConfig((LPCSTR)port_name.c_str(), &CommConfig, &size) || size > sizeof CommConfig)
			result.push_back(port_name);
	}

	return result;
}

SerialPort::SerialPort(std::string &name, SerialPortConfig &port_config)
{
	this->name = name;
	std::string win_name = "\\\\.\\" + name;

	this->port = CreateFile((LPCSTR)win_name.c_str(), GENERIC_READ | GENERIC_WRITE, 
						  0,
						  NULL,
						  OPEN_EXISTING,
						  0,
						  NULL);


	if(this->port == INVALID_HANDLE_VALUE)
	{
		DWORD lastError = GetLastError();
		printf("Error Opening %s Port, Error: %d\n", win_name.c_str(), lastError);
		return;
	}

	SetupComm(this->port, 256, 256); 

	this->config.fOutxCtsFlow = FALSE;
	this->config.fOutxDsrFlow = FALSE;
	this->config.fOutX = FALSE;
	this->config.fInX = FALSE;
	this->config.fNull = 0;
	this->config.fAbortOnError = 0;

	this->config.BaudRate = std::stoi(port_config.Speed);
	this->config.ByteSize = std::stoi(port_config.Databits);

	if(port_config.Parity == "Odd")
		this->config.Parity = ODDPARITY;
	else if(port_config.Parity == "Even")
		this->config.Parity = EVENPARITY;
	else
		this->config.Parity = NOPARITY;

	if(port_config.Stopbits == "1")
		this->config.StopBits = ONESTOPBIT;
	// else if(port_config.StopBits == "1.5")
	// 	this->config.StopBits = ONE5STOPBITS;	
	else
		this->config.StopBits = TWOSTOPBITS;

	COMMTIMEOUTS timeouts = {MAXDWORD, 0, 20, 0, 0};
	if(!SetCommTimeouts(this->port, &timeouts) || !SetCommState(this->port, &this->config))
	{
		printf("Error initialization %s Port\n", name.c_str());
		CloseHandle(this->port);
	}

	PurgeComm(this->port, PURGE_RXABORT | PURGE_TXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}

SerialPort::~SerialPort()
{
	CloseHandle(this->port);
}

std::string SerialPort::GetName()
{
	return this->name;
}

void SerialPort::Write(char* buff, int len)
{
	DWORD written_len;
	uint8_t *pos = (uint8_t*)buff;
	
	while(len > 0)
	{
		if(!WriteFile(this->port, pos, len, &written_len, NULL))
			break;
			
		if (written_len < 1)
			break;
	
		len -= written_len;
		pos += written_len;
	}
}

int SerialPort::Read(char* buff)
{
	DWORD readed_len = 1;
	int len = 0;

	while(readed_len > 0)
	{

		ReadFile(this->port, (buff + len), 1, &readed_len, NULL);
		if(readed_len > 0) len++;
	}
		
	return len;	
}
