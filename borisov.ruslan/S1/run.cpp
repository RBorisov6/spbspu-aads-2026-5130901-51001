#include "run.hpp"
#include "list.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <cstdlib>

namespace borisov {

void run(std::istream& in, std::ostream& out, std::ostream& err) {
  using SequenceList = List< std::pair<std::string, List<int> > >;

  SequenceList sequences;

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream iss(line);
    std::string name;
    iss >> name;
    if (name.empty()) {
      continue;
    }
    List<int> numbers;
    int num;
    while (iss >> num) {
      numbers.push_back(num);
    }
    if (iss.fail()) {
      err << "Error: invalid number format" << std::endl;
      std::exit(1);
    }
    sequences.push_back(std::make_pair(name, numbers));
  }

  if (sequences.empty()) {
    out << "0\n";
    return;
  }

  SequenceList::iterator seq_it = sequences.begin();
  while (seq_it != sequences.end()) {
    if (seq_it != sequences.begin()) {
      out << ' ';
    }
    out << seq_it->first;
    ++seq_it;
  }
  out << '\n';

  std::size_t max_len = 0;
  seq_it = sequences.begin();
  while (seq_it != sequences.end()) {
    if (seq_it->second.size() > max_len) {
      max_len = seq_it->second.size();
    }
    ++seq_it;
  }

  if (max_len == 0) {
    out << "0\n";
    return;
  }

  List< List<int> > columns;
  for (std::size_t i = 0; i < max_len; ++i) {
    List<int> col;
    seq_it = sequences.begin();
    while (seq_it != sequences.end()) {
      if (i < seq_it->second.size()) {
        List<int>::iterator num_it = seq_it->second.begin();
        std::size_t j = 0;
        while (j < i) {
          ++num_it;
          ++j;
        }
        col.push_back(*num_it);
      }
      ++seq_it;
    }
    columns.push_back(col);
  }

  List<int> sums;
  List< List<int> >::iterator col_it = columns.begin();
  while (col_it != columns.end()) {
    int sum = 0;
    List<int>::iterator num_it = col_it->begin();
    while (num_it != col_it->end()) {
      if (num_it != col_it->begin()) {
        out << ' ';
      }
      out << *num_it;
      sum += *num_it;
      ++num_it;
    }
    out << '\n';
    sums.push_back(sum);
    ++col_it;
  }

  List<int>::iterator sum_it = sums.begin();
  while (sum_it != sums.end()) {
    if (sum_it != sums.begin()) {
      out << ' ';
    }
    out << *sum_it;
    ++sum_it;
  }
  out << '\n';
}

}
