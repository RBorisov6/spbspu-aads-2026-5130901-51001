#include "tfidf.hpp"
#include <cmath>

namespace borisov
{
  void buildTfidf(Corpus& corpus)
  {
    for (auto dit = corpus.docs_.begin(); dit != corpus.docs_.end(); ++dit)
    {
      dit->weights_.clear();
      double total = 0.0;
      for (auto wit = dit->words_.cbegin(); wit != dit->words_.cend(); ++wit)
      {
        safeAdd(dit->weights_, *wit, 1.0);
        total += 1.0;
      }
      if (total > 0.0)
      {
        for (auto eit = dit->weights_.begin(); eit != dit->weights_.end(); ++eit)
        {
          eit->second /= total;
        }
      }
    }
    WeightMap df(64);
    for (auto dit = corpus.docs_.cbegin(); dit != corpus.docs_.cend(); ++dit)
    {
      for (auto eit = dit->weights_.cbegin(); eit != dit->weights_.cend(); ++eit)
      {
        safeAdd(df, eit->first, 1.0);
      }
    }
    double N = static_cast< double >(corpus.docs_.size());
    corpus.idf_.clear();
    for (auto it = df.begin(); it != df.end(); ++it)
    {
      double idf = std::log((N + 1.0) / (it->second + 1.0)) + 1.0;
      safeAdd(corpus.idf_, it->first, idf);
    }
    for (auto dit = corpus.docs_.begin(); dit != corpus.docs_.end(); ++dit)
    {
      for (auto eit = dit->weights_.begin(); eit != dit->weights_.end(); ++eit)
      {
        if (corpus.idf_.has(eit->first))
        {
          eit->second *= corpus.idf_.at(eit->first);
        }
      }
    }
    corpus.tfidf_built_ = true;
    corpus.centroids_.clear();
    for (auto dit = corpus.docs_.begin(); dit != corpus.docs_.end(); ++dit)
    {
      dit->cluster_ = -1;
    }
  }

  double avgVectorLength(const Corpus& corpus)
  {
    if (corpus.docs_.empty())
    {
      return 0.0;
    }
    double total = 0.0;
    for (auto dit = corpus.docs_.cbegin(); dit != corpus.docs_.cend(); ++dit)
    {
      total += static_cast< double >(dit->weights_.size());
    }
    return total / static_cast< double >(corpus.docs_.size());
  }
}
