#ifndef StringConversion_HeaderPlusPlus
#define StringConversion_HeaderPlusPlus
#include <sstream>
#include <string>

template<typename To>
struct ConvTo
{
	static To f(const std::string &str)
	{
		std::istringstream ss (str);
		To temp;
		ss >> temp;
		return(temp);
	}
};
template<>
struct ConvTo<signed char>
{
	static signed char f(const std::string &str)
	{
		std::istringstream ss (str);
		signed short temp;
		ss >> temp;
		return(signed char(temp));
	}
};
template<>
struct ConvTo<unsigned char>
{
	static signed char f(const std::string &str)
	{
		std::istringstream ss (str);
		unsigned short temp;
		ss >> temp;
		return(unsigned char(temp));
	}
};
template<>
struct ConvTo<std::string>
{
	template<typename From>
	static std::string f(From num)
	{
		std::ostringstream ss;
		ss << num;
		return(ss.str());
	}
	template<>
	static std::string f<signed char>(signed char num)
	{
		std::ostringstream ss;
		ss << signed short(num);
		return(ss.str());
	}
	template<>
	static std::string f<unsigned char>(unsigned char num)
	{
		std::ostringstream ss;
		ss << unsigned short(num);
		return(ss.str());
	}
};

#endif