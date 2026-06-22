#ifndef BORISOV_FILEUTIL_HPP
#define BORISOV_FILEUTIL_HPP

#include "list.hpp"
#include <string>

namespace borisov
{
  List< std::string > loadWords(const std::string& path);
  List< std::string > listTxtFiles(const std::string& dir);
  List< std::string > loadStopwords(const std::string& path);
  bool directoryExists(const std::string& path);
}

#endif
