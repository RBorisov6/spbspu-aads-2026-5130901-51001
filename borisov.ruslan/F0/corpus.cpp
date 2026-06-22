#include "corpus.hpp"

void borisov::safeAdd(borisov::WeightMap& m, const std::string& k, double v)
{
  if (m.size() >= m.slots() * 3 / 4)
  {
    m.rehash(m.slots() * 2 + 1);
  }
  if (m.has(k))
  {
    m.at(k) += v;
  }
  else
  {
    m.add(k, v);
  }
}

std::size_t borisov::Corpus::docCount() const
{
  return docs_.size();
}
