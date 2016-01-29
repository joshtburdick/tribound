/** Writes out a formula specifying a triangle-finding
  circuit of a given size. Using Tommi Junttila's bc2cnf
  program, this can be translated to a CNF formulat (in
  the usual DIMACS form), and fed to a SAT solver.

  FIXME I probably don't need to write out as many parenthesis
  (e.g. since the precedence of ~ seems to be what you'd expect),
  but I'll leave them in for now anyway. */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <cstdlib>

using namespace std;

/** Writes out a formula corresponding to a bound.
  All indices are 0-based.
  Note that for each possible "input schema",
  we need to write out a different copy of the circuit.
  Different "circuits" (or "cases") will be denoted by
  prefixing variables with "C[number]". */
struct BoundWriter {

  /** These give the problem size. */
  size_t numVertices, numGates;

  /** Names of inputs. */
  vector<string> inputNames;

  /** Counter of which "case" we're currently writing out
    (essentially which copy of the circuit.) */
  size_t c;

  /** Edges of a random graph (for adding a random constraints.)
    We only use the entries of g[i][j] with i < j. */
  vector<vector<bool> > g;

  /** Constructor. */
  BoundWriter(size_t _numVertices, size_t _numGates) {
    numVertices = _numVertices;
    numGates = _numGates;
    c = 0;

    // record names of input wires
    for(size_t i=0; i<numVertices; i++)
      for(size_t j=i+1; j<numVertices; j++) {
        stringstream s;
        s << "e_" << i << "_" << j;
        inputNames.push_back(s.str());
      }

    // pre-allocate g
    g.resize(numVertices);
    for(size_t i=0; i<numVertices; i++)
      g[i].resize(numVertices);
  }

  /** Writes out the definitions of what each gate computes.
    Wires are named "w_[to]_[from]", where [to] is "g_[n]",
    and [from] is either "g_[n]" or "e_[i]_[j]". Note that
    the input graph and gate outputs are case-specific,
    but the wires are not (as the same circuit needs to work
    in all cases.)
    NOTE: this also increments the "case" counter c, and so
    for a given case, this should be called last. */
  void writeGateDefs() {
    for(size_t g=0; g<numGates; g++) {
      cout << "C_" << c << "_" << "g_" << g << " := ~(T" << endl;

      // wires from the inputs.
      for (size_t i=0; i<inputNames.size(); i++) {
        string a = inputNames[i];
        cout << " & (w_g_" << g << "_" << a <<
          " => " << "C_" << c << "_" << a << ")" << endl;
      }

      // wires from the other gates
      for (size_t h=0; h<g; h++) {
        cout << " & (w_g_" << g << "_g_" << h <<
          " => C_" << c << "_g_" << h << ")" << endl;
      }

      cout << ");" << endl;
    }

    c++;
  }

  /** Writes out inputs which have just one triangle on. */
  void writeTriangleCases() {

    // loop through the triangle
    for(size_t i=0; i<numVertices; i++)
      for(size_t j=i+1; j<numVertices; j++)
        for(size_t k=j+1; k<numVertices; k++) {

          // write what inputs are 1 in the triangle
          for(size_t a=0; a<numVertices; a++)
            for(size_t b=a+1; b<numVertices; b++) {
              string s = ((a==i && b==j) ||
                (a==i && b==k) ||
                (a==j && b==k)) ? "" : "~";
              cout << "ASSIGN " << s << "C_" << c << "_e_" << a << "_" << b << ";" << endl;
            }

          // ... and assert that the circuit should output a 1
          cout << "ASSIGN C_" << c << "_g_" << (numGates-1) << ";" << endl;

          // write out the gate definitions
          writeGateDefs();
        }
  }

  /** Writes out inputs which are subgraphs of CBIPs
    (and therefore definitely lack triangles) */
  void writeCBIPsubsetCases() {

    // loop through the subsets (we only loop through half
    // of them, because the others are symmetric)
    size_t numVertexSets = 1u << numVertices;
    for(size_t s = 1; s < numVertexSets / 2; s++) {

      // feed in 0 to each edge which has both ends in the same set
      for(size_t i = 0; i < numVertices; i++)
        for(size_t j = i+1; j < numVertices; j++) {
          bool v1 = (s & (1u << i)) > 0;
          bool v2 = (s & (1u << j)) > 0;
          string s = v1 == v2 ? "~" : "";
          cout << "ASSIGN " << s << "C_" << c << "_e_" << i << "_" << j << ";" << endl;
        }

      // ... and assert that the output is 0
      cout << "ASSIGN ~C_" << c << "_g_" << (numGates-1) << ";" << endl;

      // write out the circuit, yet again
      writeGateDefs();
    }
  }

  /** Writes constraints based on random circuits. In theory,
    including enough of these should be equivalent to including
    all possible input graphs. It's unclear how many will be
    needed to give a reasonable bound.
    edgeProb - probability of an edge being present. */
  void writeRandomGraphConstraint(double edgeProb) {

    // pick edges of g, randomly with some probability
    for(size_t i=0; i<numVertices; i++)
      for(size_t j=1+1; j<numVertices; j++)
        g[i][j] = drand48() <= edgeProb;    // FIXME use better RNG

    // check if there's a triangle
    bool hasTriangle = false;
    for(size_t i=0; i<numVertices; i++)
      for(size_t j=i+1; j<numVertices; j++)
        for(size_t k=j+1; k<numVertices; k++)
          if (g[i][j] && g[i][k] && g[j][k])
            hasTriangle = true;
    // XXX should break out of loop if there's a triangle, but whatever

    // write constraints on input
    for(size_t i=0; i<numVertices; i++)
      for(size_t j=i+1; j<numVertices; j++) {
        string s = g[i][j] ? "" : "~";
        cout << "ASSIGN " << s << "C_" << c << "_e_" << i << "_" << j << ";" << endl;
      }

    // and constraint on output
    string s = hasTriangle ? "" : "~";
    cout << "ASSIGN " << s << "C_" << c << "_g_" << (numGates-1) << ";" << endl;

    // lastly, write out the circuit (for this case)
    writeGateDefs();
  }

  /** Gets the variable names corresponding to the wires for one
    gate. */
  vector<string> getInputVars(size_t g) {
    vector<string> a;

    // start with the inputs to this gate
    for(size_t i = 0; i < inputNames.size(); i++) {
      stringstream s;
      s << "w_g_" << g << "_" << inputNames[i];
      a.push_back(s.str());
    }

    // then add connections from other gates
    for(size_t j = 0; j < g; j++) {
      stringstream s;
      s << "w_g_" << g << "_g_" << j;
      a.push_back(s.str());
    }

    return a;
  }

  /** Writes a constraint that bitstring a is strictly less than
    bitstring b. The bitstrings are represented as vectors of
    variable names (or expressions), with LSB at index 0.

    This assumes that a is either the same length as b, or shorter,
    and that b is at least two bits long. If a is shorter than b,
    then a's most significant bits are padded with 0's.

    The resulting formula is named p (this will also write some
    intermediate formulas, whose names will be prefixed with p. */
  void writeLessThan(string p, vector<string> a, vector<string> b) {
    size_t n = b.size();

    // pad with a with 0's, if necessary
    while (a.size() < b.size())
      a.push_back("F");

    // write out initial cases of "less than"
    cout << p << "_l_" << (n-1)
      << " := (~" << a[n-1] << ") & " << b[n-1] << ";" << endl;
    // ... and "equal"
    cout << p << "_e_" << (n-1)
      << " := ~(" << a[n-1] << " ^ " << b[n-1] << ");" << endl;

    // loop through the other bits
    for(size_t i = n-2; i >= 1; i--) {

      // first, formula for "this prefix is strictly less"
      cout << p << "_l_" << i << " := " <<
        p << "_l_" << (i+1) << " | " << 
        "((~" << a[i] << ") & " << b[i] << ");" << endl;

      // then, formula for "the earlier bits are all equal"
      cout << p << "_e_" << i << " := " <<
        p << "_e_" << (i+1) << " & " << 
        "(~(" << a[i] << " ^ " << b[i] << "));" << endl;
    }

    // lastly, write the "less than" formula
    cout << p << " := " <<
        p << "_l_1 | " << 
        "((~" << a[0] << ") & " << b[0] << ");" << endl;
  }

  /** Writes constraint that edges are lexicographically sorted.
    (It's not clear whether this will help or not.) */
  void writeWiresSorted() {

    // assert that adjacent input wires are successively increasing
    // (by transitivity, this forces the whole list to be sorted)
    for(size_t g = 1; g < numGates; g++) {
      stringstream s;
      s << "wires_sorted_" << (g-1) << "_" << g;
      writeLessThan(s.str(), getInputVars(g-1), getInputVars(g));
    }
  }

  /** Writes out the entire formula. */
  void writeFormula(size_t numRandomCases) {
    cout << "BC1.0" << endl;

    writeTriangleCases();
    writeCBIPsubsetCases();
    for(size_t i = 0; i < numRandomCases; i++)
      writeRandomGraphConstraint(0.5); 
  }

};

int main(int argc, char** argv) {
  if (argc != 5) {
    cerr << "Writes a formula specifying a NAND-gate-finding subcircuit" << endl;
    cerr << "note: arguments aren't checked for errors" << endl;
    cerr << "Usage: writeBoundFormula1 N G RANDOM_CASES WIRES_SORTED, where" << endl;
    cerr << "N - number of vertices" << endl;
    cerr << "G - number of NAND gates" << endl;
    cerr << "RANDOM_CASES - number of random graphs to include" << endl;
    cerr << "WIRES_SORTED - if 1, then the rows of the adjacency matrix of which" << endl;
    cerr << "  wires are connected will be sorted" << endl;
    exit(1);
  }

  BoundWriter bw(atoi(argv[1]), atoi(argv[2]));
  bw.writeFormula(atoi(argv[3]));
  if (atoi(argv[4]) > 0)
    bw.writeWiresSorted();
  return 0;
}

