#ifndef BORISOV_IO_HPP
#define BORISOV_IO_HPP

#include "bstree.hpp"
#include <iosfwd>
#include <string>

namespace borisov
{
  using Dict = BSTree< int, std::string >;
  using DataTable = BSTree< std::string, Dict >;

  bool readDicts(std::istream& in, DataTable& table);
}

#endif
