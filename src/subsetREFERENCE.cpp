#include "utils.h"
#include <unordered_map>

#if __cplusplus <= 199711L
#error This library needs at least a C++11 compliant compiler
#endif

class snp {
public:
  int pos;
  string ref, alt;

  snp(int pos, string ref, string alt) {
    this->pos = pos;
    this->ref = ref;
    this->alt = alt;
  }

  ~snp() {}

  bool strand(string &r, string &a) {
    if (ref == r && alt == a)
      return true;
    return false;
  }
};

int main(int argc, char **argv) {
  string buffer;

  cerr << "subsetREFERENCE v1.1.2" << endl;
  if (argc < 4) {
    cerr << "Incorrect number of options:\nsubsetREFERENCE "
            "subsetSites.map reference.haps/.gen(.gz) output.file.gen(.gz) "
            "[complement|noStrand]" << endl;
    exit(1);
  }
  bool complement = false;
  bool noStrand = false;
  for (int i = 4; i < argc; ++i) {
    if (strcmp(argv[i], "complement") == 0) {
      complement = true;
      cerr << "Output is complement of subset\n";
    }
    if (strcmp(argv[i], "noStrand") == 0) {
      noStrand = true;
      cerr << "Allowing ref/alt flip for match\n";
    }
  }

  int line = 0;
  unordered_multimap<string, snp> M;
  cerr << "Reading subset of variants in [" << argv[1] << "]" << endl;
  ifile fdl(argv[1]);
  while (getline(fdl, buffer, '\n')) {
    vector<string> tokens = sutils::tokenize(buffer, " ", 6);
    if (tokens.size() < 5)
      throw runtime_error("Input map line has less than 5 columns [" + buffer +
                          "]");
    std::string::size_type last_char = 0;
    int pos = stoi(tokens[1], &last_char);
    if (last_char != tokens[1].size())
      throw runtime_error("Could not parse position [" + tokens[1] + "]");

    M.insert(pair<string, snp>(tokens[0] + ":" + tokens[1],
                               snp(pos, tokens[3], tokens[4])));
    line++;
  }
  cerr << "  * " << line << " variants read" << endl;
  fdl.close();
  if (line < 1) {
    cerr << "ERROR: input .map is empty" << endl;
    exit(1);
  }

  line = 0;
  int found = 0;
  cerr << "Reading haplotype file in [" << argv[2]
       << "] and writing subset in [" << argv[3] << "]" << endl;
  ifile fdi(argv[2]);
  ofile fdo(argv[3]);

  // start off begin in the past so we get first update right away
  clock_t begin = clock() - CLOCKS_PER_SEC * 10;
  while (getline(fdi, buffer, '\n')) {

    clock_t end = clock();
    // refresh every ten seconds
    if (double(end - begin) / CLOCKS_PER_SEC > 10) {
      cerr << "\r" << found << " / " << line;
      begin = clock();
    }

    vector<string> tokens = sutils::tokenize(buffer, " ", 6);
    string a0 = tokens[3];
    string a1 = tokens[4];
    bool ok = false;

    // attempt to match a0 and a1 with an element from M
    auto seqM = M.equal_range(tokens[0] + ":" + tokens[2]);
    for (auto it = seqM.first; it != seqM.second; ++it) {
      // also check the reverse strand if nostrand is true
      ok = it->second.strand(a0, a1) || (noStrand && it->second.strand(a1, a0));
      if (ok)
        break;
    }
    if (complement)
      ok = !ok;
    if (ok) {
      fdo << buffer << endl;
      ++found;
    }
    line++;
  }
  cerr << "\r" << found << " / " << line << endl;
  fdo.close();
  fdi.close();
}
