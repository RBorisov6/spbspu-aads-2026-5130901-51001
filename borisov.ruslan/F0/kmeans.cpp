#include "kmeans.hpp"
#include <cmath>
#include <ostream>

namespace borisov
{
  double cosineSim(const WeightMap& a, const WeightMap& b)
  {
    double dot = 0.0;
    double normA = 0.0;
    double normB = 0.0;
    for (auto it = a.cbegin(); it != a.cend(); ++it)
    {
      normA += it->second * it->second;
      if (b.has(it->first))
      {
        dot += it->second * b.at(it->first);
      }
    }
    for (auto it = b.cbegin(); it != b.cend(); ++it)
    {
      normB += it->second * it->second;
    }
    if (normA <= 0.0 || normB <= 0.0)
    {
      return 0.0;
    }
    return dot / (std::sqrt(normA) * std::sqrt(normB));
  }

  static void initCentroids(Corpus& corpus)
  {
    corpus.centroids_.clear();
    int count = 0;
    for (auto it = corpus.docs_.cbegin(); it != corpus.docs_.cend() && count < corpus.k_; ++it, ++count)
    {
      corpus.centroids_.pushBack(it->weights_);
    }
  }

  static int assignClusters(Corpus& corpus)
  {
    int changed = 0;
    for (auto dit = corpus.docs_.begin(); dit != corpus.docs_.end(); ++dit)
    {
      int best = 0;
      double bestSim = -1.0;
      int ci = 0;
      for (auto cit = corpus.centroids_.cbegin(); cit != corpus.centroids_.cend(); ++cit, ++ci)
      {
        double sim = cosineSim(dit->weights_, *cit);
        if (sim > bestSim)
        {
          bestSim = sim;
          best = ci;
        }
      }
      if (dit->cluster_ != best)
      {
        dit->cluster_ = best;
        ++changed;
      }
    }
    return changed;
  }

  static void updateCentroids(Corpus& corpus)
  {
    int k = corpus.k_;
    corpus.centroids_.clear();
    for (int ci = 0; ci < k; ++ci)
    {
      WeightMap centroid(64);
      int count = 0;
      for (auto dit = corpus.docs_.cbegin(); dit != corpus.docs_.cend(); ++dit)
      {
        if (dit->cluster_ != ci)
        {
          continue;
        }
        for (auto eit = dit->weights_.cbegin(); eit != dit->weights_.cend(); ++eit)
        {
          safeAdd(centroid, eit->first, eit->second);
        }
        ++count;
      }
      if (count > 0)
      {
        for (auto eit = centroid.begin(); eit != centroid.end(); ++eit)
        {
          eit->second /= static_cast< double >(count);
        }
      }
      corpus.centroids_.pushBack(centroid);
    }
  }

  void runKmeans(Corpus& corpus, int maxIter, std::ostream& out)
  {
    initCentroids(corpus);
    for (auto dit = corpus.docs_.begin(); dit != corpus.docs_.end(); ++dit)
    {
      dit->cluster_ = -1;
    }
    for (int iter = 1; iter <= maxIter; ++iter)
    {
      int changed = assignClusters(corpus);
      out << "iter " << iter << ": assignments changed " << changed << '\n';
      if (changed == 0)
      {
        out << "<CLUSTERING CONVERGED after " << iter
            << " iterations in '" << corpus.name_ << "'>\n";
        return;
      }
      updateCentroids(corpus);
    }
    out << "<CLUSTERING CONVERGED after " << maxIter
        << " iterations in '" << corpus.name_ << "'>\n";
  }
}
