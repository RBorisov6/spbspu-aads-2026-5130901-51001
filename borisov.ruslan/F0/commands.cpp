#include "commands.hpp"
#include "fileutil.hpp"
#include "tfidf.hpp"
#include "kmeans.hpp"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>

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
      { "build-tfidf",      cmdBuildTfidf },
      { "list-corpuses",    cmdListCorpuses },
      { "delete-corpus",    cmdDeleteCorpus },
    };
    const std::size_t CMD_COUNT = sizeof(CMD_TABLE) / sizeof(CMD_TABLE[0]);
  }

  void executeCommand(const std::string& line, CorpusTable& table, std::ostream& out)
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
}
