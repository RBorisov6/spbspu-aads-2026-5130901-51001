#include "run.hpp"
#include "list.hpp"
#include <iostream>
#include <climits>
#include <limits>

namespace borisov
{

const char * ERR_MSG =
  "Formed lists with exit code 1 and error message in standard error because of overflow\n";

struct Sequence
{
  std::string name;
  List< int > nums;

  explicit Sequence(const std::string & n):
    name(n)
  {}
};

void skipSpaces(std::istream & in)
{
  while (in.peek() == ' ' || in.peek() == '\t')
  {
    in.get();
  }
}

bool isEndOfLine(std::istream & in)
{
  int c = in.peek();
  return c == '\n' || c == '\r' || c == std::istream::traits_type::eof();
}

void skipLine(std::istream & in)
{
  while (!isEndOfLine(in))
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

bool checkedSum(unsigned long long a,
  unsigned long long b,
  unsigned long long & result)
{
  if (b > std::numeric_limits< unsigned long long >::max() - a)
  {
    return true;
  }

  result = a + b;
  return false;
}

bool readInput(std::istream & in,
  std::ostream & err,
  List< Sequence > & sequences)
{
  std::string name;

  while (in >> name)
  {
    Sequence sequence(name);
    skipSpaces(in);

    while (!isEndOfLine(in))
    {
      unsigned long long value = 0;

      if (in >> value)
      {
        if (value > static_cast< unsigned long long >(INT_MAX))
        {
          err << ERR_MSG;
          return true;
        }

        sequence.nums.push_back(static_cast< int >(value));
      }
      else
      {
        in.clear();

        if (!isEndOfLine(in))
        {
          err << ERR_MSG;
          return true;
        }

        break;
      }
    }

    skipLine(in);
    sequences.push_back(sequence);
  }

  return false;
}

void outputNames(const List< Sequence > & sequences,
  std::ostream & out)
{
  LCIter< Sequence > it = sequences.cbegin();
  bool isFirst = true;

  for (; it != sequences.cend(); ++it)
  {
    if (!isFirst)
    {
      out << ' ';
    }

    out << it->name;
    isFirst = false;
  }

  out << '\n';
}

bool outputNumbers(const List< Sequence > & sequences,
  std::ostream & out,
  std::ostream & err)
{
  if (sequences.empty())
  {
    return false;
  }

  std::size_t maxLength = 0;

  for (LCIter< Sequence > it = sequences.cbegin();
    it != sequences.cend();
    ++it)
  {
    if (it->nums.size() > maxLength)
    {
      maxLength = it->nums.size();
    }
  }

  if (maxLength == 0)
  {
    out << "0\n";
    return false;
  }

  List< List< int > > columns;

  for (std::size_t col = 0; col < maxLength; ++col)
  {
    List< int > column;

    for (LCIter< Sequence > seqIt = sequences.cbegin();
      seqIt != sequences.cend();
      ++seqIt)
    {
      if (col < seqIt->nums.size())
      {
        LCIter< int > numIt = seqIt->nums.cbegin();

        for (std::size_t i = 0; i < col; ++i)
        {
          ++numIt;
        }

        column.push_back(*numIt);
      }
    }

    columns.push_back(column);
  }

  for (LCIter< List< int > > colIt = columns.cbegin();
    colIt != columns.cend();
    ++colIt)
  {
    bool isFirst = true;

    for (LCIter< int > numIt = colIt->cbegin();
      numIt != colIt->cend();
      ++numIt)
    {
      if (!isFirst)
      {
        out << ' ';
      }

      out << *numIt;
      isFirst = false;
    }

    out << '\n';
  }

  List< unsigned long long > sums;

  for (LCIter< List< int > > colIt = columns.cbegin();
    colIt != columns.cend();
    ++colIt)
  {
    unsigned long long sum = 0;

    for (LCIter< int > numIt = colIt->cbegin();
      numIt != colIt->cend();
      ++numIt)
    {
      unsigned long long value =
        static_cast< unsigned long long >(*numIt);

      if (checkedSum(sum, value, sum))
      {
        err << ERR_MSG;
        return true;
      }
    }

    if (sum > static_cast< unsigned long long >(INT_MAX))
    {
      err << ERR_MSG;
      return true;
    }

    sums.push_back(sum);
  }

  if (sums.empty())
  {
    out << "0\n";
  }
  else
  {
    bool isFirst = true;

    for (LCIter< unsigned long long > it = sums.cbegin();
      it != sums.cend();
      ++it)
    {
      if (!isFirst)
      {
        out << ' ';
      }

      out << *it;
      isFirst = false;
    }

    out << '\n';
  }

  return false;
}

int run(std::istream & in,
  std::ostream & out,
  std::ostream & err)
{
  List< Sequence > sequences;

  if (readInput(in, err, sequences))
  {
    return 1;
  }

  if (sequences.empty())
  {
    out << "0\n";
    return 0;
  }

  outputNames(sequences, out);

  if (outputNumbers(sequences, out, err))
  {
    return 1;
  }

  return 0;
}

}
