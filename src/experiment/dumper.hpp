#ifndef EXPERIMENT_DUMPER_HPP
#define EXPERIMENT_DUMPER_HPP 1
#include <boost/program_options.hpp>
#include <array>
#include <set>
#include <fstream>

namespace po = boost::program_options;
using std::array;
namespace experiment {
class dumper {
  private:
    const std::set<std::string> m_allowed_dumps;
    po::options_description m_options;
  public:
    dumper(): m_allowed_dumps({"edges","itemsets","items","rules","symbols","terminals","nonterminals"}){
      m_options.add_options()
        ("help", "print a help message")
        ("file", po::value<std::ofstream>(), "dump to specific file")
        ("what", po::value<std::vector<std:string>>()->multitokens(), "what kind of content you want to dump")
        ;
    }
    void dump(int argc, char **argv) {
      po::variable_map vmap;
      po::store(po::parse(argc, argv, m_options), vmap);
      po::notify(vmap);
    }
};
} //namespace experiment
#endif // EXPERIMENT_DUMPER_HPP
