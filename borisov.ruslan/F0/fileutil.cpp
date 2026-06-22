#include "fileutil.hpp"
#include <fstream>
#include <cctype>
#include <dirent.h>
#include <sys/stat.h>

namespace borisov
{
  List< std::string > loadWords(const std::string& path)
  {
    List< std::string > words;
    std::ifstream f(path);
    if (!f)
    {
      return words;
    }
    std::string token;
    while (f >> token)
    {
      std::string word;
      for (std::size_t i = 0; i < token.size(); ++i)
      {
        unsigned char c = static_cast< unsigned char >(token[i]);
        if (std::isalpha(c))
        {
          word += static_cast< char >(std::tolower(c));
        }
      }
      if (!word.empty())
      {
        words.pushBack(word);
      }
    }
    return words;
  }

  List< std::string > listTxtFiles(const std::string& dir)
  {
    List< std::string > files;
    DIR* d = opendir(dir.c_str());
    if (!d)
    {
      return files;
    }
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr)
    {
      std::string name(ent->d_name);
      if (name.size() > 4 && name.substr(name.size() - 4) == ".txt")
      {
        files.pushBack(dir + "/" + name);
      }
    }
    closedir(d);
    return files;
  }

  List< std::string > loadStopwords(const std::string& path)
  {
    List< std::string > sw;
    std::ifstream f(path);
    if (!f)
    {
      return sw;
    }
    std::string line;
    while (std::getline(f, line))
    {
      std::string word;
      for (std::size_t i = 0; i < line.size(); ++i)
      {
        unsigned char c = static_cast< unsigned char >(line[i]);
        if (!std::isspace(c))
        {
          word += static_cast< char >(std::tolower(c));
        }
      }
      if (!word.empty())
      {
        sw.pushBack(word);
      }
    }
    return sw;
  }

  bool directoryExists(const std::string& path)
  {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
  }
}
