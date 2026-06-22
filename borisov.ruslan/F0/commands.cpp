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
      { "load-corpus",   cmdLoadCorpus },
      { "list-corpuses", cmdListCorpuses },
      { "delete-corpus", cmdDeleteCorpus },
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
