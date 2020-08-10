
#ifndef COMMON_STUFFS
#define COMMON_STUFFS

#include <string>
#include <vector>

//std::experimental::filesystem::path::preferred_separator which should return either / or \ depending on your platform.
#ifdef WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#ifdef WIN32
#include <ciso646> // and and or keyword support :)
#endif

#define CSTOI(x) (x.size() == 0 ? 0 : std::stoi(x)) // Check stoi !-)
inline int CSTOUL(const std::string &s, int defaultValue) { try { return std::stoul(s); } catch (...) { return defaultValue; } } // This is empty string and negative number proof

// Source = http://www.cplusplus.com/forum/general/33669/
template <typename T, unsigned size>
inline unsigned sizeOfArray(const T(&)[size]) { return size; }

enum class Target { screen, midiIn, midiOut, misc, last_item }; //TODO: Refine midi by channel ?
const std::string targets[(int) Target::last_item] = { "screen", "midiIn", "midiOut", "misc" }; // Don't forget the leading const :)
void SetLogFilter(bool value, Target t);
void SetLogFolder(const std::string &value);

std::string IntToStr(int value); // So Delphi...
std::string RunningFolder();
//bool fileExist(const char *fullFileName);
bool FileExists(const std::string &fullFileName);
std::string StringUpper(const std::string& s);
void Join(std::string& dest, const std::string& item, const std::string& delimiter); // Little string accumulator helper
std::vector<std::string> Split(const std::string& str, const std::string& delimiter); // Almost the opposite :)
void LogThis(const char *s, Target t);
void LogThis2(const char *format, Target t, ...);
std::string AppendPath(const std::vector<std::string>& pieces);
std::string Trim(const std::string &value); // Prefix string related function names with String ?
void FindAndReplaceInternal(std::string &s, const std::string &what, const std::string &by);
std::string FindAndReplace(const std::string &s, const std::string &what, const std::string &by);
std::string LeftDollar(const std::string &s, const std::string& subs);
std::string RightDollar(const std::string &s, const std::string& subs);
bool StringStartsWith(const std::string &s, const std::string& subs);
bool StringEndsWith(const std::string &s, const std::string& subs);
bool StringInString(const std::string &s, const std::string& subs);
std::string StringTrim(const std::string &s);

#endif // COMMON_STUFFS
