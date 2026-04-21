#include "run.hpp"
#include "list.hpp"
#include <iostream>
#include <sstream>
#include <climits>
#include <limits>
#include <cstdlib>

namespace borisov {

struct Sequence {
  std::string name;
  List<int> nums;
  explicit Sequence(const std::string& n) : name(n) {}
};

void getWithoutSkips(std::istream& in) {
  while (in.peek() == ' ' || in.peek() == '\t') in.get();
}

bool isEnd(std::istream& in) {
  int c = in.peek();
  return c == '\n' || c == '\r' || c == std::istream::traits_type::eof();
}

void skipLine(std::istream& in) {
  while (!isEnd(in)) in.get();
  if (in.peek() == '\r') in.get();
  if (in.peek() == '\n') in.get();
}

bool checkedSum(unsigned long long a, unsigned long long b, unsigned long long& res) {
  if (b > std::numeric_limits<unsigned long long>::max() - a) return true;
  res = a + b;
  return false;
}

void readInput(std::istream& in, std::ostream& err, List<Sequence>& seqs) {
  std::string name;
  while (in >> name) {
    Sequence seq(name);
    getWithoutSkips(in);
    while (!isEnd(in)) {
      unsigned long long num = 0;
      if (in >> num) {
        if (num > static_cast<unsigned long long>(INT_MAX)) {
          err << "Error: number out of int range" << std::endl;
          std::exit(1);
        }
        seq.nums.push_back(static_cast<int>(num));
      } else {
        in.clear();
        if (!isEnd(in)) {
          err << "Error: invalid number format" << std::endl;
          std::exit(1);
        }
        break;
      }
    }
    skipLine(in);
    seqs.push_back(std::move(seq));
  }
}

void outputNames(const List<Sequence>& seqs, std::ostream& out) {
  LCIter<Sequence> it = seqs.cbegin();
  bool first = true;
  for (; it != seqs.cend(); ++it) {
    if (!first) out << ' ';
    out << it->name;
    first = false;
  }
  out << '\n';
}

void outputNums(const List<Sequence>& seqs, std::ostream& out, std::ostream& err) {
  if (seqs.empty()) return;
  std::size_t max_len = 0;
  for (LCIter<Sequence> curr = seqs.cbegin(); curr != seqs.cend(); ++curr) {
    if (curr->nums.size() > max_len) max_len = curr->nums.size();
  }
  if (max_len == 0) {
    out << "0\n";
    return;
  }
  List<List<int>> columns;
  for (std::size_t col = 0; col < max_len; ++col) {
    List<int> column;
    for (LCIter<Sequence> seq_it = seqs.cbegin(); seq_it != seqs.cend(); ++seq_it) {
      if (col < seq_it->nums.size()) {
        LCIter<int> num_it = seq_it->nums.cbegin();
        for (std::size_t i = 0; i < col; ++i) ++num_it;
        column.push_back(*num_it);
      }
    }
    columns.push_back(std::move(column));
  }
  for (LCIter<List<int>> col_it = columns.cbegin(); col_it != columns.cend(); ++col_it) {
    bool first_in_col = true;
    for (LCIter<int> num_it = col_it->cbegin(); num_it != col_it->cend(); ++num_it) {
      if (!first_in_col) out << ' ';
      out << *num_it;
      first_in_col = false;
    }
    out << '\n';
  }
  List<unsigned long long> sums;
  bool overflow = false;
  for (LCIter<List<int>> col_it = columns.cbegin(); col_it != columns.cend(); ++col_it) {
    unsigned long long sum = 0;
    for (LCIter<int> num_it = col_it->cbegin(); num_it != col_it->cend(); ++num_it) {
      unsigned long long val = static_cast<unsigned long long>(*num_it);
      if (checkedSum(sum, val, sum)) {
        err << "Error: sum overflow" << std::endl;
        overflow = true;
        break;
      }
    }
    if (overflow) break;
    if (sum > static_cast<unsigned long long>(INT_MAX)) {
      err << "Error: sum out of int range" << std::endl;
      std::exit(1);
    }
    sums.push_back(sum);
  }
  if (overflow) std::exit(1);
  if (sums.empty()) {
    out << "0\n";
  } else {
    bool first_sum = true;
    for (LCIter<unsigned long long> sum_it = sums.cbegin(); sum_it != sums.cend(); ++sum_it) {
      if (!first_sum) out << ' ';
      out << *sum_it;
      first_sum = false;
    }
    out << '\n';
  }
}

void run(std::istream& in, std::ostream& out, std::ostream& err) {
  List<Sequence> sequences;
  readInput(in, err, sequences);
  if (sequences.empty()) {
    out << "0\n";
    return;
  }
  outputNames(sequences, out);
  outputNums(sequences, out, err);
}

}
