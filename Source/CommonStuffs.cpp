
#include "CommonStuffs.hpp"
#include "ConfigurationFiles.hpp"

#include <algorithm>
#include <thread>
#include <iostream>
#include <iomanip>
#include <ctime>
// Include milli second... Nice but it doesn't comple :( ? See https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
// https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
//#include <sys/time.h>
#include <chrono>
// See https://stackoverflow.com/questions/19555121/how-to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
#include <sstream>
#include <cstdarg>
#include <string>
#include <mutex>

std::mutex mtxlog; // Soyons fou :) Add more thread safety to the logger

// New tentative to get the milliseconds :)
// https://stackoverflow.com/questions/10654258/get-millisecond-part-of-time/10655106
#ifdef WIN32
#define localtime_r(_Time, _Tm) localtime_s(_Tm, _Time)
#endif

// C++17 only : defined in the upcoming C++ 17 standard
// #include <filesystem>
//std::string path = std::filesystem::current_path();

// See https://stackoverflow.com/questions/143174/how-do-i-get-the-directory-that-a-program-is-running-from
// In the meantime :
//#include <experimental/filesystem>
//std::string path = std::experimental::filesystem::current_path();

#ifdef WIN32 // Don't use WINDOWS !
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// Can't understand why there is no shorter way to perform this conversion...
// In C++ 2011 there is finally std::to_string() !!! The opposite is std::stoi()
std::string IntToStr(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

// Source for both : https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c/19841704

#ifdef _WIN32 // What is the difference with WIN32 ?
#include <io.h>
#define access _access_s
#else
#include <unistd.h>
#endif

bool FileExists(const std::string &fullFileName)
{
	return access(fullFileName.c_str(), 0) == 0;
}

// Doesn't compile... Why ?
//bool fileExists(const char *fullFileName)
//{
//	std::ifstream inputFile(fullFileName);
//	return inputFile.good();
//}

// Actually, this is more getCurrentFolder and we need to get the running app folder
std::string CurrentFolder()
{
	char cCurrentPath[FILENAME_MAX];

	if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
	{
		return ".";
	}

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
	return std::string(cCurrentPath);
}

std::string StringUpper(const std::string &s)
{
  std::string result(s.size(), 0);

  std::transform(s.begin(), s.end(), result.begin(), [] (unsigned char c) { return toupper(c); } ); // Yes a lambda :)

  return result;
}

std::string AppendPath(const std::vector<std::string> &pieces)
{
	std::string result;

	for (const std::string &piece : pieces)
	{
		result += piece; // auto usage was ok here but not in the line below :(
		if (piece.back() != PATH_SEPARATOR)
		{
			result += PATH_SEPARATOR;
		}
	}

	//std::cout << "result = " << result << std::endl;
	result.erase(result.end()-1); // Remove the trailing path separator :-)
	//std::cout << "result2 = " << result << std::endl;

	return result;
}

void Join(std::string &dest, const std::string &item, const std::string &delimiter)
{
	if (dest.size() > 0)
	{
		dest += delimiter;
	}

  dest += item;
}

// Source = https://stackoverflow.com/questions/13172158/c-split-string-by-line
std::vector<std::string> Split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> results;

    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    while ((pos = str.find(delimiter, prev)) != std::string::npos)
    {
        results.push_back(str.substr(prev, pos - prev));
        prev = pos + delimiter.size();
    }

    // To get the last substring (or only, if delimiter is not found)
    results.push_back(str.substr(prev));

    return results;
}

// Source : https://stackoverflow.com/questions/150543/forward-an-invocation-of-a-variadic-function-in-c
// And also care about the last named parameter for the call to va_start :
// https://stackoverflow.com/questions/13189365/how-to-remove-this-warning-second-parameter-of-va-start-not-last-named-argume
void LogThis2(const char *format, Target t, ...)
{
	char s[512]; // char[512] s; is ok in C# :)
	va_list args;
	va_start(args, t);
	vsnprintf(s, sizeof(s), format, args);
	LogThis(s, t);
	va_end(args);
}

// In Rack/plugins/samlp_sandbox :
//
// Log formatted... Almost similar to printf :) But the "..." format string cannot be empty !-(
// #define LOGF(s, ...) logf(stringf(s, __VA_ARGS__).c_str())
//
// C like simple logger
// void logf(const char *s);

bool logFilters[(int) Target::last_item];
void SetLogFilter(bool value, Target t)
{
	logFilters[(int) t] = value;
}

std::string logFolder = CurrentFolder();
void SetLogFolder(const std::string &value)
{
	logFolder = value;
}

std::string getCurrentTimestamp()
{
	using std::chrono::system_clock;
	auto currentTime = std::chrono::system_clock::now();
	char buffer[80];

	auto transformed = currentTime.time_since_epoch().count() / 1000000;
	auto millis = transformed % 1000;

	std::time_t tt;
	tt = system_clock::to_time_t(currentTime);
	auto timeinfo = localtime(&tt);
	strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
	sprintf(buffer, "%s.%03d", buffer, (int) millis);

	return std::string(buffer);
}

void LogThis(const char *s, Target t)
{
	if (!logFilters[(int) t])
	{
		return;
	}

	/*
	time_t rawtime;

  
	struct tm* timeinfo;
	//struct timeval tmnow;
	char timestamp[32]; // Sure there is more C++ way to do this...

	// Not C++ ?
	//gettimeofday(&tmnow, NULL);
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", timeinfo);
	*/

	std::unique_lock<std::mutex> lck(mtxlog, std::defer_lock);
	lck.lock();

	std::string fullFilename = AppendPath({ logFolder, targets[(int)t] }) + ".log";
	//std::cout << "fullFilename = " << fullFilename << std::endl;
	FILE *fLog = fopen(fullFilename.c_str(), "a");
	// We can't assume the underlying type of the thread id... Possibly not an int !
	// See https://stackoverflow.com/questions/7432100/how-to-get-integer-thread-id-in-c11
	// std::thread::id tid = std::this_thread::get_id();
	std::stringstream ss;
	ss << std::this_thread::get_id(); // Value is not in hex (and not garanteed to be unique in course of the process lifetime)
	fprintf(fLog, "%s (TID=%s) => %s\n", /* timestamp */ getCurrentTimestamp().c_str(), ss.str().c_str(), s);
	fclose(fLog);

	lck.unlock();
	// No need for a finally clause thanks to the std::unique_lock... Anyway, it does not exist in C++ :)
	// https://stackoverflow.com/questions/161177/does-c-support-finally-blocks-and-whats-this-raii-i-keep-hearing-about
}

// Waiting for the C++17 standard... See
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string Trim(const std::string &value)
{
	std::string result;

	std::stringstream ss;
	ss << value;
	ss >> result;

	return result;
}

// https://stackoverflow.com/questions/4643512/replace-substring-with-another-substring-c
void FindAndReplaceInternal(std::string& s, const std::string &what, const std::string &by)
{

	if ((s.length() == 0) || (what.length() == 0) || (by.length() == 0))
	{
		return;
	}

	size_t idx = 0;
	for (;;)
	{
		idx = s.find(what, idx);
		if (idx == std::string::npos)
		{
			return;
		}

		s.replace(idx, what.length(), by);
		idx += by.length();
	}
}

std::string FindAndReplace(const std::string& s, const std::string &what, const std::string &by)
{
	std::string result(s);

	FindAndReplaceInternal(result, what, by);

	return result;
}

// We definetively need a new StringStuff unit !
std::string LeftDollar(const std::string &s, const std::string &subs)
{
	size_t index = s.find(subs);
	if (index == std::string::npos)
	{
		return "";
	}
	else
	{
		return s.substr(0, index);
	}	
}

std::string RightDollar(const std::string &s, const std::string &subs)
{
	size_t index = s.find(subs);
	if (index == std::string::npos)
	{
		return "";
	}
	else
	{
		return s.substr(index + subs.size());
	}
}

// Unable to extend the string class...
bool StringStartsWith(const std::string &s, const std::string &subs)
{
	return (s.find(subs, 0) != std::string::npos);
}

bool StringEndsWith(const std::string &s, const std::string &subs)
{
	return (s.rfind(subs, std::string::npos) != std::string::npos);
}

bool StringInString(const std::string &s, const std::string &subs)
{
	return (s.find(subs) != std::string::npos);
}

// Waiting for the C++17 standard... See
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string StringTrim(const std::string &value)
{
	std::string result;
	std::stringstream ss;

	ss << value;
	ss >> result;

	return result;
}
