#ifndef RCP_DFA
#define RCP_DFA

#include <vector>
#include <string>
#include <rcp/nfa.hpp>
#include <ostream>
#include <ext/hash_map>

/** DFATransition represents a transition (an edge) in a DFA.
  * It contains the destination, input and
  * most unusual output of the transition
  * (one for each NFA node in the destination)
  */
class DFATransition // {{{
{
  public:
    DFATransition();
    DFATransition(int dest, const std::map<int,std::pair<std::string,int> > &output);
    virtual ~DFATransition();

    int GetDest() const;
    const std::string &GetOutput(int dest, int &source) const;
    std::string ToString(int source, char input) const;

  private:
   int myDest;
   std::map<int,std::pair<std::string,int> > myOutput;
}; // }}}

/** DFANode represents a node in a DFA.
  * It contains the transitions from the node,
  * and the information concerning the node (if it is final)
  */
class DFANode // {{{
{
  public:
    DFANode(const std::set<int> &nfanodes);
    virtual ~DFANode();

    // Final determins if the node is a final state
    // Value -1 means it is not a final state
    // Values >= 0 means it is a final state,
    // and the value is the used final state from the NFA
    int Final() const;
    void SetFinal(int final);
    void AddTransition(char input, const DFATransition &trans);
    std::set<char> GetEdges() const;
    const DFATransition &GetTransition(char input) const;
    const std::set<int> &GetNFANodes() const;
    std::string ToString(int id) const;

  private:
   int myFinal;
   std::set<int> myNfaNodes;
   __gnu_cxx::hash_map<char, DFATransition> myTransitions;
}; // }}}

/** DFA represents a Nondeterministic Finite Automaton
  */
class DFA // {{{
{ public:
    DFA(const NFA &nfa, bool precompile=true);
    virtual ~DFA();

    // AddNode adds a node to the DFA, and returns the id of the node
    int AddNode(const std::set<int> &nfanodes);
    int CountNodes();
    // Accept checks if a string is accepted by the NFA
    bool Accept(const std::string &s);
    // Compress returns a compact bit-representation of the given string, and
    // throws a string error if the given string is not accepted
    std::string Compress(const std::string &s);
    // Parse returns a RV representing the given string, and
    // throws a string error if the given string is not accepted
    RV *Parse(const std::string &s);
    // ToString prints the NFA in the dot format for easy debugging
    std::string ToString() const;
    // Compile generates C-code to parse strings to compact bit representation
    std::string Compile() const;
    void Compile(std::ostream &dest) const;


  private:
    // Create constructs DFA nodes with transitions
    // equivalent to the given NFA nfa, starting at the start node.
    // The return value contains the final node. 
    void Create(int pos);
    int CreateNode(int pos, char input);

    std::vector<DFANode> myNodes;
    std::map<int,std::string> myInitCodes;
    NFA myNfa;
}; // }}}

#endif

