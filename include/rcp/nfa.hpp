#ifndef RCP_NFA
#define RCP_NFA

#include <vector>
#include <string>
#include <set>
#include <rcp/bitcode.hpp>
#include <rcp/bitcode_order.hpp>
#include <rcp/re.hpp>

/** NFATransition represents a transition (an edge) in a NFA.
  * It contains the destination, input and
  * most unusual output of the transition (as used in Dube and Feeley)
  */
class NFATransition // {{{
{
  public:
    NFATransition(int dest, const std::string &input, const std::string &output);
    virtual ~NFATransition();

    int GetDest() const;
    const std::string &GetInput() const;
    const std::string &GetOutput() const;

  private:
   int myDest;
   std::string myInput;
   std::string myOutput;
}; // }}}

/** NFANode represents a node in a NFA.
  * It contains the transitions from the node,
  * and the information concerning the node (if it is final)
  */
class NFANode // {{{
{
  public:
    NFANode();
    virtual ~NFANode();

    bool Final() const;
    void SetFinal(bool final);
    void AddTransition(const NFATransition &trans);
    void RemoveTransition(int id);
    int CountTransitions() const;
    const NFATransition &GetTransition(int id) const;

  private:
   bool myFinal;
   std::vector<NFATransition> myTransitions;
}; // }}}

/** NFA represents a Nondeterministic Finite Automaton
  */
class NFA // {{{
{ public:
    typedef std::map<int,BitCode> thompson_state;

    NFA(const RE &exp);
    virtual ~NFA();

    // AddNode adds a node to the NFA, and returns the id of the node
    int AddNode();
    int CountNodes() const;
    const NFANode &GetNode(int i) const;
    // Deterministic checks if there is a node with conflicting edges
    bool Deterministic() const;
    // Accept checks if a string is accepted by the NFA
    bool Accept(const std::string &s) const;
    // Compress returns a compact bit-representation of the given string, and
    // throws a string error if the given string is not accepted
    BitCode Compress(const std::string &s) const;
    // Thompson simulation based parsing
    // Not as efficient as DFASIM, but returns the least bitcode for the given ordering.
    // This enables greedy-leftmost and longest-leftmost disambiguations
    BitCode Thompson(const std::string &s, const BCCState &init_state, const BCComparer leq) const;
    // Parse returns a RV representing the given string, and
    // throws a string error if the given string is not accepted
    RV *Parse(const std::string &s) const;
    // Find the epsilon-closure of a set of nodes in the NFA
    // The structure of the elementa are (dest,(output,source))
    void Closure(std::map<int,std::pair<std::string,int> > &nodes) const;
    void Closure(thompson_state &nodes, BCCStates &bccstate, const BCComparer leq) const;
    // ToString prints the NFA in the dot format for easy debugging
    std::string ToString();

    // Methods for the out-deterministic NFA reduction algorithm
    void RemoveDeadStates();
    void RemoveUnreachableStates();
    void MakeCompact();
    void Reduce();
    void Explode();
    // FindPrefixes requires no dead states. Otherwise it may not terminate
    std::vector<std::string> FindPrefixes();
    void MakePrefix();

  private:
    bool Accept(const std::string &s, int pos, std::set<int> marked) const;
    bool Compress(const std::string &s, int pos, BitCode &dest, std::set<int> marked) const;
    std::vector<NFATransition> CompactEdges(const NFATransition &edge);
    // Create constructs NFA nodes representing exp, starting at the start
    // node. The return value contains the final node. 
    int Create(const RE &exp, int start);

    std::vector<NFANode> myNodes;
}; // }}}

#endif
