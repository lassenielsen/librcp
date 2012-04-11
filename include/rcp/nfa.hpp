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
    /*** The generalized thompson state holds a bitcode and an integer for each node reached at each index
       * The bitcode holds the bits passed since the state of the previous string index
       * The integer defines the nfa-node from the previous string index
       */
    typedef std::map<int,std::pair<BitCode, int> > thompson_state;
    /*** Map from index, node to bitcode, prev-node, or rather from index to a
       * thompson state
       */
    typedef std::map<int,thompson_state> thompson_states;

    /*** The constructor generates the nodes and edges of the automaton in a
       * way similar to the thompson-construction, but also adds output to the
       * edges in order to generate bitcodes during traversal.
       */
    NFA(const RE &exp);
    /*** Destructor
       */
    virtual ~NFA();

    // Deterministic checks if there is a node with conflicting edges
    bool Deterministic() const;
    // Accept checks if a string is accepted by the NFA
    bool Accept(const std::string &s) const;
    // Compress returns a compact bit-representation of the given string, and
    // throws a string error if the given string is not accepted
    BitCode Compress(const std::string &s) const;
    /*** Thompson simulation based parsing
       * Not as efficient as DFASIM, but returns the least bitcode for the given ordering.
       * This enables greedy-leftmost and longest-leftmost disambiguations
       */
    BitCode Thompson(const std::string &s, BCCState *bcc_init, BCComparer leq, BCUpdater bcc_update) const;
    /*** Parse returns a RV representing the given string, and
       * throws a string error if the given string is not accepted.
       * Parse is is based on a backtracking algorithm.
       */
    RV *Parse(const std::string &s) const;
    /*** Closure performs an epsilon-closure on the given set of nodes. This is
       * used in many algorithms, such as thompson simulation and dfa subset
       * construction.
       * The structure of the elements are (dest,(output,source))
       */
    void Closure(std::map<int,std::pair<std::string,int> > &nodes) const;
    /*** Closure is used for the generalized thompson simulation, to performa
       * an epsilon closure, selecting the leas possible bitcodes for each
       * reachable node.
       * For efficiency reasons, this version of the closure algorithm uses a
       * breath first traversal, using a bit more memory, but avoiding a
       * potential exponential time-usage.
       */
    void Closure(thompson_state &nodes, BCCStates &bccstate, const BCComparer leq) const;
    /*** ToString returns a text-representation of automaton that can be
       * rendered using dot.
       */
    std::string ToString();

    /*** RemoveDeadStates detects and removes dead states from the automaton.
       */
    void RemoveDeadStates();
    /*** RemoveUnreachableStates detects and removes unreachable states from
       * the automaton.
       */
    void RemoveUnreachableStates();
    /*** MakeCompact eliminates epsilon/epsilon edges, and ensures that only
       * the initial node can have output-epsilon edges.
       */
    void MakeCompact();
    /*** Reduce performs a transformation similar to the dfa minimization.
       */
    void Reduce();
    /*** Explode transforms the automaton to ensure that all edges have inputs
       * of length at mose one.
       */
    void Explode();
    /*** FintPrefixes returns the input-prefix for each node.
       * FindPrefixes requires no dead states. Otherwise it may not terminate!
       */
    std::vector<std::string> FindPrefixes();
    /*** MakePrefix transforms the automaton to the frefix form where all nodes
       * (except possibly the initial node) have the empty prefix
       */
    void MakePrefix();
    /*** Minimize transforms the automaton to an equivalent automaton with the
       * minimal number of nodes possible.
       * The argument allow_reorder determins if the
       * transformation can allow the reordering of inputs and
       * outputs. If allow_reorder is set to false a minimal
       * transducer preserving the input-output ordering is
       * obtained
       */
    void Minimize(bool allow_reorder=false, bool explode=true);

    // AddNode adds a node to the NFA, and returns the id of the node
    int AddNode();
    int CountNodes() const;
    const NFANode &GetNode(int i) const;

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
