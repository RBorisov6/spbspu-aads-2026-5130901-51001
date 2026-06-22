#ifndef BORISOV_CORPUS_HPP
#define BORISOV_CORPUS_HPP

#include "hashtable.hpp"
#include "xxhash.hpp"
#include "list.hpp"
#include <string>
#include <cstddef>

namespace borisov
{
  using WeightMap = HashTable< std::string, double, XxHash32, std::equal_to< std::string > >;

  void safeAdd(WeightMap& m, const std::string& k, double v);

  struct Document
  {
    std::string name_;
    List< std::string > words_;
    WeightMap weights_;
    int cluster_;

    Document():
      name_(),
      words_(),
      weights_(64),
      cluster_(-1)
    {}

    explicit Document(const std::string& name):
      name_(name),
      words_(),
      weights_(64),
      cluster_(-1)
    {}
  };

  struct Corpus
  {
    std::string name_;
    List< Document > docs_;
    WeightMap idf_;
    List< WeightMap > centroids_;
    int k_;
    bool tfidf_built_;

    Corpus():
      name_(),
      docs_(),
      idf_(64),
      centroids_(),
      k_(2),
      tfidf_built_(false)
    {}

    explicit Corpus(const std::string& n):
      name_(n),
      docs_(),
      idf_(64),
      centroids_(),
      k_(2),
      tfidf_built_(false)
    {}

    std::size_t docCount() const;
  };

  using CorpusTable = HashTable< std::string, Corpus, XxHash32, std::equal_to< std::string > >;
}

#endif
