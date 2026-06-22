#ifndef BORISOV_KMEANS_HPP
#define BORISOV_KMEANS_HPP

#include "corpus.hpp"
#include <iosfwd>

namespace borisov
{
  double cosineSim(const WeightMap& a, const WeightMap& b);
  void runKmeans(Corpus& corpus, int maxIter, std::ostream& out);
}

#endif
