
#ifndef CONFIGURATION_FILES
#define CONFIGURATION_FILES

#include <string>
#include <map> // We could also use unordered map here...
#include <vector>

class ConfigurationFile
{
private:
  std::map<std::string, std::map<std::string, std::string> > m_contents; // The "> >" means I'm old school :)
  bool m_saveChanges;
  std::string m_fullFileName;

public:
	ConfigurationFile(const std::string &fullFileName, bool scratchExisting);
	~ConfigurationFile();
	std::string Read(const std::string &section, const std::string &key, const std::string &defaultValue);
	std::vector<std::string> ReadSections();
	std::vector<std::string> ReadKeys(const std::string &section);
	void Write(const std::string &section, const std::string &key, const std::string &value);
};

#endif // CONFIGURATION_FILES
