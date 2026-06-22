#include "commands.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include "fileutil.hpp"
#include "kmeans.hpp"
#include "tfidf.hpp"

namespace borisov
{
  namespace
  {
    std::string baseName(const std::string& path)
    {
      std::size_t pos = path.rfind('/');
      if (pos == std::string::npos)
      {
        return path;
      }
      return path.substr(pos + 1);
    }

    bool getCorpus(CorpusTable& table, const std::string& name, std::ostream& out, Corpus*& ptr)
    {
      if (!table.has(name))
      {
        out << "<CORPUS NOT FOUND>\n";
        return false;
      }
      ptr = &table.at(name);
      return true;
    }

    void cmdLoadCorpus(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      std::string dir;
      if (!(in >> name >> dir))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      if (!directoryExists(dir))
      {
        out << "<DIRECTORY NOT FOUND>\n";
        return;
      }
      if (table.has(name))
      {
        table.drop(name);
      }
      if (table.size() >= table.slots() * 3 / 4)
      {
        table.rehash(table.slots() * 2 + 1);
      }
      table.add(name, Corpus(name));
      Corpus& corpus = table.at(name);
      List< std::string > files = listTxtFiles(dir);
      int count = 0;
      for (auto it = files.begin(); it != files.end(); ++it)
      {
        std::string docName = baseName(*it);
        Document doc(docName);
        doc.words_ = loadWords(*it);
        corpus.docs_.pushBack(doc);
        ++count;
      }
      out << "<CORPUS '" << name << "' CREATED: " << count << " documents>\n";
    }

    List< std::pair< std::string, double > > sortedByWeight(const WeightMap& m)
    {
      List< std::pair< std::string, double > > result;
      for (auto it = m.cbegin(); it != m.cend(); ++it)
      {
        auto pos = result.begin();
        while (pos != result.end() && pos->second >= it->second)
        {
          ++pos;
        }
        result.insert(pos, *it);
      }
      return result;
    }

    void cmdShowClusterWords(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      int cid = 0;
      int n = 0;
      if (!(in >> name >> cid >> n))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      WeightMap sum(64);
      int count = 0;
      for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
      {
        if (dit->cluster_ != cid)
        {
          continue;
        }
        for (auto eit = dit->weights_.cbegin(); eit != dit->weights_.cend(); ++eit)
        {
          safeAdd(sum, eit->first, eit->second);
        }
        ++count;
      }
      if (count > 0)
      {
        for (auto eit = sum.begin(); eit != sum.end(); ++eit)
        {
          eit->second /= static_cast< double >(count);
        }
      }
      out << "Top words for cluster " << cid << ":\n";
      List< std::pair< std::string, double > > sorted = sortedByWeight(sum);
      int shown = 0;
      bool first = true;
      for (auto sit = sorted.begin(); sit != sorted.end() && shown < n; ++sit, ++shown)
      {
        if (!first)
        {
          out << ", ";
        }
        out << "\"" << sit->first << "\" ("
            << std::fixed << std::setprecision(2) << sit->second << ")";
        first = false;
      }
      out << '\n';
    }

    void cmdFindCommon(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      int cid = 0;
      int minDocs = 0;
      if (!(in >> name >> cid >> minDocs))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      HashTable< std::string, int, XxHash32, std::equal_to< std::string > > docCount(64);
      for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
      {
        if (dit->cluster_ != cid)
        {
          continue;
        }
        for (auto eit = dit->weights_.cbegin(); eit != dit->weights_.cend(); ++eit)
        {
          if (docCount.size() >= docCount.slots() * 3 / 4)
          {
            docCount.rehash(docCount.slots() * 2 + 1);
          }
          if (docCount.has(eit->first))
          {
            docCount.at(eit->first) += 1;
          }
          else
          {
            docCount.add(eit->first, 1);
          }
        }
      }
      out << "Common words in cluster " << cid
          << " (present in >=" << minDocs << " docs):\n";
      List< std::pair< std::string, int > > qualifying;
      for (auto it = docCount.cbegin(); it != docCount.cend(); ++it)
      {
        if (it->second >= minDocs)
        {
          auto pos = qualifying.begin();
          while (pos != qualifying.end() && pos->second >= it->second)
          {
            ++pos;
          }
          qualifying.insert(pos, *it);
        }
      }
      bool first = true;
      for (auto it = qualifying.begin(); it != qualifying.end(); ++it)
      {
        if (!first)
        {
          out << ", ";
        }
        out << "\"" << it->first << "\" (" << it->second << " docs)";
        first = false;
      }
      if (!qualifying.empty())
      {
        out << '\n';
      }
    }

    void cmdWcss(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      if (!(in >> name))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      out << "WCSS for '" << name << "':\n";
      double total = 0.0;
      int ci = 0;
      for (auto cit = corpus->centroids_.cbegin(); cit != corpus->centroids_.cend(); ++cit, ++ci)
      {
        double clusterWcss = 0.0;
        for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
        {
          if (dit->cluster_ != ci)
          {
            continue;
          }
          double sim = cosineSim(dit->weights_, *cit);
          clusterWcss += 1.0 - sim;
        }
        out << "Cluster " << ci << ": "
            << std::fixed << std::setprecision(2) << clusterWcss << '\n';
        total += clusterWcss;
      }
      out << "Total: " << std::fixed << std::setprecision(2) << total << '\n';
    }

    void cmdSaveModel(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      std::string filename;
      if (!(in >> name >> filename))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      std::ofstream f(filename);
      if (!f)
      {
        out << "<CANNOT OPEN FILE>\n";
        return;
      }
      f << "K " << corpus->k_ << '\n';
      int ci = 0;
      for (auto cit = corpus->centroids_.cbegin(); cit != corpus->centroids_.cend(); ++cit, ++ci)
      {
        f << "CENTROID " << ci << '\n';
        for (auto eit = cit->cbegin(); eit != cit->cend(); ++eit)
        {
          f << eit->first << ' ' << eit->second << '\n';
        }
        f << "CENTROID_END\n";
      }
      f << "ASSIGNMENTS\n";
      for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
      {
        f << dit->name_ << ' ' << dit->cluster_ << '\n';
      }
      out << "<MODEL SAVED to " << filename << ">\n";
    }

    void cmdLoadModel(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      std::string filename;
      if (!(in >> name >> filename))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      std::ifstream f(filename);
      if (!f)
      {
        out << "<FILE NOT FOUND>\n";
        return;
      }
      std::string token;
      int k = 0;
      if (!(f >> token >> k) || token != "K")
      {
        out << "<INVALID MODEL FILE>\n";
        return;
      }
      corpus->k_ = k;
      corpus->centroids_.clear();
      while (f >> token)
      {
        if (token == "ASSIGNMENTS")
        {
          break;
        }
        if (token != "CENTROID")
        {
          continue;
        }
        int cid = 0;
        f >> cid;
        WeightMap centroid(64);
        std::string line;
        std::getline(f, line);
        while (std::getline(f, line))
        {
          if (line == "CENTROID_END")
          {
            break;
          }
          std::istringstream ss(line);
          std::string word;
          double weight = 0.0;
          if (ss >> word >> weight)
          {
            if (centroid.size() >= centroid.slots() * 3 / 4)
            {
              centroid.rehash(centroid.slots() * 2 + 1);
            }
            centroid.add(word, weight);
          }
        }
        corpus->centroids_.pushBack(centroid);
      }
      std::string docName;
      int clusterIdx = 0;
      while (f >> docName >> clusterIdx)
      {
        for (auto dit = corpus->docs_.begin(); dit != corpus->docs_.end(); ++dit)
        {
          if (dit->name_ == docName)
          {
            dit->cluster_ = clusterIdx;
            break;
          }
        }
      }
      out << "<MODEL LOADED into '" << name << "': " << k << " clusters>\n";
    }

    void cmdShowClusters(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      if (!(in >> name))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      for (int ci = 0; ci < corpus->k_; ++ci)
      {
        int size = 0;
        for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
        {
          if (dit->cluster_ == ci)
          {
            ++size;
          }
        }
        out << "Cluster " << ci << " (size=" << size << "):";
        bool first = true;
        for (auto dit = corpus->docs_.cbegin(); dit != corpus->docs_.cend(); ++dit)
        {
          if (dit->cluster_ != ci)
          {
            continue;
          }
          if (!first)
          {
            out << ",";
          }
          out << " " << dit->name_;
          first = false;
        }
        out << '\n';
      }
    }

    void cmdShowCentroids(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      if (!(in >> name))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      int ci = 0;
      for (auto cit = corpus->centroids_.cbegin(); cit != corpus->centroids_.cend(); ++cit, ++ci)
      {
        out << "Centroid " << ci << ":";
        List< std::pair< std::string, double > > sorted = sortedByWeight(*cit);
        int shown = 0;
        for (auto sit = sorted.begin(); sit != sorted.end() && shown < 5; ++sit, ++shown)
        {
          if (shown > 0)
          {
            out << ",";
          }
          out << " \"" << sit->first << "\" ("
              << std::fixed << std::setprecision(2) << sit->second << ")";
        }
        out << '\n';
      }
    }

    void cmdSetK(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      int k = 0;
      if (!(in >> name >> k))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      if (k < 2)
      {
        out << "<INVALID K (k < 2)>\n";
        return;
      }
      if (static_cast< std::size_t >(k) > corpus->docs_.size())
      {
        out << "<INVALID K (k > document count)>\n";
        return;
      }
      corpus->k_ = k;
      corpus->centroids_.clear();
      for (auto dit = corpus->docs_.begin(); dit != corpus->docs_.end(); ++dit)
      {
        dit->cluster_ = -1;
      }
      out << "<K SET TO " << k << " for '" << name << "'>\n";
    }

    void cmdCluster(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      int maxIter = 0;
      if (!(in >> name >> maxIter))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      if (!corpus->tfidf_built_)
      {
        out << "<TF-IDF NOT BUILT. Run build-tfidf first.>\n";
        return;
      }
      if (static_cast< std::size_t >(corpus->k_) > corpus->docs_.size())
      {
        out << "<INVALID K (k > document count)>\n";
        return;
      }
      runKmeans(*corpus, maxIter, out);
    }

    void cmdBuildTfidf(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      if (!(in >> name))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      buildTfidf(*corpus);
      double avg = avgVectorLength(*corpus);
      out << "<TF-IDF BUILT for '" << name
          << "': vocabulary=" << corpus->idf_.size()
          << ", documents=" << corpus->docs_.size()
          << ", avg vector length=" << std::fixed << std::setprecision(1) << avg << ">\n";
    }

    void cmdAddDocument(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      std::string filename;
      if (!(in >> name >> filename))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      std::string docName = baseName(filename);
      Document doc(docName);
      doc.words_ = loadWords(filename);
      corpus->docs_.pushBack(doc);
      corpus->tfidf_built_ = false;
      out << "<ADDED '" << docName << "' to '" << name << "'>\n";
    }

    void cmdRemoveStopwords(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      std::string swFile;
      if (!(in >> name >> swFile))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      Corpus* corpus = nullptr;
      if (!getCorpus(table, name, out, corpus))
      {
        return;
      }
      List< std::string > sw = loadStopwords(swFile);
      if (!sw.empty())
      {
        HashTable< std::string, int, XxHash32, std::equal_to< std::string > > swSet(
          static_cast< std::size_t >(sw.size() * 2 + 1)
        );
        for (auto it = sw.begin(); it != sw.end(); ++it)
        {
          if (!swSet.has(*it))
          {
            swSet.add(*it, 1);
          }
        }
        for (auto dit = corpus->docs_.begin(); dit != corpus->docs_.end(); ++dit)
        {
          List< std::string > filtered;
          for (auto wit = dit->words_.begin(); wit != dit->words_.end(); ++wit)
          {
            if (!swSet.has(*wit))
            {
              filtered.pushBack(*wit);
            }
          }
          dit->words_ = filtered;
        }
        corpus->tfidf_built_ = false;
      }
      out << "<STOPWORDS REMOVED from '" << name << "'. Run build-tfidf to update.>\n";
    }

    void cmdListCorpuses(std::istream&, std::ostream& out, CorpusTable& table)
    {
      if (table.empty())
      {
        out << '\n';
        return;
      }
      bool first = true;
      for (auto it = table.cbegin(); it != table.cend(); ++it)
      {
        if (!first)
        {
          out << ", ";
        }
        out << it->first << " (" << it->second.docs_.size() << " docs)";
        first = false;
      }
      out << '\n';
    }

    void cmdDeleteCorpus(std::istream& in, std::ostream& out, CorpusTable& table)
    {
      std::string name;
      if (!(in >> name))
      {
        out << "<INVALID COMMAND>\n";
        return;
      }
      if (!table.has(name))
      {
        out << "<CORPUS NOT FOUND>\n";
        return;
      }
      table.drop(name);
      out << "<CORPUS '" << name << "' DELETED>\n";
    }

    using CmdFunc = void(*)(std::istream&, std::ostream&, CorpusTable&);

    struct CmdEntry
    {
      const char* name;
      CmdFunc func;
    };

    const CmdEntry CMD_TABLE[] = {
      { "load-corpus",      cmdLoadCorpus },
      { "add-document",     cmdAddDocument },
      { "remove-stopwords", cmdRemoveStopwords },
      { "set-k",            cmdSetK },
      { "cluster",          cmdCluster },
      { "show-clusters",      cmdShowClusters },
      { "show-centroids",     cmdShowCentroids },
      { "show-cluster-words", cmdShowClusterWords },
      { "find-common",        cmdFindCommon },
      { "wcss",               cmdWcss },
      { "save-model",         cmdSaveModel },
      { "load-model",         cmdLoadModel },
      { "build-tfidf",      cmdBuildTfidf },
      { "list-corpuses",    cmdListCorpuses },
      { "delete-corpus",    cmdDeleteCorpus },
    };
    const std::size_t CMD_COUNT = sizeof(CMD_TABLE) / sizeof(CMD_TABLE[0]);
  }
}

void borisov::executeCommand(const std::string& line, borisov::CorpusTable& table, std::ostream& out)
{
  std::istringstream ss(line);
  std::string cmd;
  if (!(ss >> cmd))
  {
    return;
  }
  for (std::size_t i = 0; i < CMD_COUNT; ++i)
  {
    if (cmd == CMD_TABLE[i].name)
    {
      CMD_TABLE[i].func(ss, out, table);
      return;
    }
  }
  out << "<INVALID COMMAND>\n";
}
