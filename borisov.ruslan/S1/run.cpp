#include "run.hpp"
#include "list.hpp"
#include <limits>
#include <iostream>
#include <string>
#include <utility>

namespace borisov
{
  struct NamedSeq
  {
    std::string name;
    List< std::size_t > nums;
  };

  namespace
  {
    void skipSpaces(std::istream& in)
    {
      while (in.peek() == ' ' || in.peek() == '\t')
      {
        in.get();
      }
    }

    bool isLineEnd(std::istream& in)
    {
      int c = in.peek();
      return c == '\n' || c == '\r' || c == std::istream::traits_type::eof();
    }

    void skipLine(std::istream& in)
    {
      while (!isLineEnd(in))
      {
        in.get();
      }
      if (in.peek() == '\r')
      {
        in.get();
      }
      if (in.peek() == '\n')
      {
        in.get();
      }
    }

    bool addChecked(std::size_t a, std::size_t b, std::size_t& result)
    {
      if (b > std::numeric_limits< std::size_t >::max() - a)
      {
        return false;
      }
      result = a + b;
      return true;
    }
  }

  List< NamedSeq > readInput(std::istream& in)
  {
    List< NamedSeq > seqs;
    std::string name;
    while (in >> name)
    {
      NamedSeq seq;
      seq.name = name;
      skipSpaces(in);
      while (!isLineEnd(in))
      {
        std::size_t value = 0;
        in >> value;
        seq.nums.pushBack(value);
        skipSpaces(in);
      }
      skipLine(in);
      seqs.pushBack(std::move(seq));
    }
    return seqs;
  }

  void printNames(std::ostream& out, const List< NamedSeq >& seqs)
  {
    bool first = true;
    for (auto it = seqs.cbegin(); it != seqs.cend(); ++it)
    {
      if (!first)
      {
        out << ' ';
      }
      out << it->name;
      first = false;
    }
    out << '\n';
  }

  int printSequences(std::ostream& out, std::ostream& err,
                     const List< NamedSeq >& seqs)
  {
    using ConstIter = typename List< std::size_t >::const_iterator;

    List< ConstIter > iters;
    for (auto s = seqs.cbegin(); s != seqs.cend(); ++s)
    {
      iters.pushBack(s->nums.cbegin());
    }

    List< List< std::size_t > > rows;
    List< std::size_t > sums;
    bool overflow = false;

    while (true)
    {
      bool hasRow = false;
      List< std::size_t > row;
      std::size_t sum = 0;
      bool rowOverflow = false;

      auto it = iters.begin();
      auto s = seqs.cbegin();

      while (it != iters.end())
      {
        if (*it != s->nums.cend())
        {
          hasRow = true;
          std::size_t value = **it;
          row.pushBack(value);

          if (!rowOverflow)
          {
            std::size_t nextSum = 0;
            if (!addChecked(sum, value, nextSum))
            {
              rowOverflow = true;
              overflow = true;
            }
            else
            {
              sum = nextSum;
            }
          }
          ++(*it);
        }
        ++it;
        ++s;
      }

      if (!hasRow)
      {
        break;
      }

      rows.pushBack(std::move(row));
      if (!rowOverflow)
      {
        sums.pushBack(sum);
      }
    }

    for (auto r = rows.cbegin(); r != rows.cend(); ++r)
    {
      bool first = true;
      for (auto v = r->cbegin(); v != r->cend(); ++v)
      {
        if (!first)
        {
          out << ' ';
        }
        out << *v;
        first = false;
      }
      out << '\n';
    }

    if (overflow)
    {
      err << "overflow\n";
      return 1;
    }

    if (sums.empty())
    {
      out << "0\n";
      return 0;
    }

    bool first = true;
    for (auto s = sums.cbegin(); s != sums.cend(); ++s)
    {
      if (!first)
      {
        out << ' ';
      }
      out << *s;
      first = false;
    }
    out << '\n';

    return 0;
  }

  int run(std::istream& in, std::ostream& out, std::ostream& err)
  {
    List< NamedSeq > seqs = readInput(in);

    if (seqs.empty())
    {
      out << "0\n";
      return 0;
    }

    printNames(out, seqs);
    return printSequences(out, err, seqs);
  }
}
