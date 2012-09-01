#ifndef Logger_HeaderPlusPlus
#define Logger_HeaderPlusPlus

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>	//Do not look directly at the evilness!
#include <Windows.h>

#include "StringConvert.hpp"

struct Logger
{
	enum LogType{ Log = 0, Error = -1/*, Standard = 1*/ };
private:
	LogType type;
	std::string file;
	bool newline;

	Logger();
	Logger(const Logger &);
	Logger &operator=(const Logger &);
	static void SetErrorColor(unsigned short c)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_ERROR_HANDLE), c);
	}
public:
	static void SetLogColor(unsigned short c)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
	}
	Logger(Logger::LogType Type, std::string File) : type(Type), file(File), newline(true) {}

	Logger &operator<<(const char *s)
	{
		std::ofstream fout (file.c_str(), std::ios::app|std::ios::out);
		if(type == Logger::Error)
		{
			SetErrorColor(12);
		}
		if(newline)
		{
			char t[51];
			time_t temp = time(0);
			strftime(t, 50, "%x %X", localtime(&temp));	//Do not look directly at the evilness!
			(type == Logger::Log ? std::clog : std::cerr) << "[" << t << "] ";
			fout << "[" << t << "] ";
			newline = false;
		}
		(type == Logger::Log ? std::clog : std::cerr) << s;
		fout << s;
		return(*this);
	}
	Logger &operator<<(const std::string &s)
	{
		return(this->operator<<(s.c_str()));
	}
	Logger &operator<<(long long int n)
	{
		return((*this).operator<<(ConvTo<std::string>::f(n).c_str()));
	}
	Logger &operator<<(std::ostream&(*f)(std::ostream&))
	{
		std::ofstream fout (file.c_str(), std::ios::app|std::ios::out);
		(type == Logger::Log ? std::clog : std::cerr) << f;
		fout << f;
		if(f == std::endl)
		{
			newline = true;
			SetLogColor(7);
		}
		return(*this);
	}

	~Logger()
	{
		if(type == Logger::Log) //No duplicate for Logger::Error
		{
			(*this) << "End of Session" << std::endl;
		}
	}
};

#endif