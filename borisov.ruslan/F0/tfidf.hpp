#ifndef BORISOV_TFIDF_HPP
#define BORISOV_TFIDF_HPP

#include "corpus.hpp"

namespace borisov
{
  void buildTfidf(Corpus& corpus);
  double avgVectorLength(const Corpus& corpus);
}

#endif
