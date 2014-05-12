#include "utils.h"

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

  if (argc < 4 || argc > 5) {
    cerr << "Incorrect number of options:\nsubsetREFERENCE "
            "reference.haps/.gen(.gz) subsetSites.map output.file.gen(.gz) "
            "[complement]" << endl;
    exit(1);
  }
  const bool complement = argc == 5 ? true : false;
  int line = 0;
  multimap<int, snp> M;
  cerr << "Reading subset of variants in [" << argv[1] << "]" << endl;
  ifile fdl(argv[1]);
  while (getline(fdl, buffer, '\n')) {
    vector<string> tokens = sutils::tokenize(buffer, " ", 6);
    int pos = atoi(tokens[1].c_str());
    M.insert(pair<int, snp>(pos, snp(pos, tokens[3], tokens[4])));
    line++;
  }
  cerr << "  * " << line << " variants read" << endl;
  fdl.close();

  line = 0;
  int found = 0;
  cerr << "Reading haplotype file in [" << argv[2]
       << "] and writing subset in [" << argv[3] << "]" << endl;
  ifile fdi(argv[2]);
  ofile fdo(argv[3]);
  while (getline(fdi, buffer, '\n')) {
    vector<string> tokens = sutils::tokenize(buffer, " ", 6);
    int pos = atoi(tokens[2].c_str());
    string a0 = tokens[3];
    string a1 = tokens[4];
    bool ok = false;
    pair<multimap<int, snp>::iterator, multimap<int, snp>::iterator> seqM =
        M.equal_range(pos);
    for (multimap<int, snp>::iterator it = seqM.first; it != seqM.second && !ok;
         ++it)
      ok = (ok || it->second.strand(a0, a1));
    if (complement)
        ok = !ok;
    if (ok) {
      fdo << buffer << endl;
      found++;
    }
    if (line % 1000 == 0) {
      cerr << "\r" << found << " / " << line;
      cerr.flush();
    }
    line++;
  }
  cerr << "\r" << found << " / " << line << endl;
  fdo.close();
  fdi.close();
}
