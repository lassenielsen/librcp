#ifndef RCP_BCORDER
#define RCP_BCORDER

#include <vector>
#include <map>
#include <string>
#include <utility>
#include <rcp/re.hpp>

class BCCState // {{{
{ public:
    BCCState();
    virtual ~BCCState();
    virtual std::string ToString() const;
  private:
}; // }}}
// Definition of GL order:
// inl x < inr y
// fold x < nil
// + context rules
class BCCState_GL : public BCCState // {{{
{ public:
    BCCState_GL();
    virtual ~BCCState_GL();
    int state; // -1=less, 0=equal, 1=greater
    BitCode lhsBuffer, rhsBuffer;
    std::string ToString() const;
}; // }}}
// Definition of LL order:
// inl x < inl y if x < y
// inr x < inr y if x < y
// iinl x < inr y
// xy < zw if |str(x)| > |str(z)|
// xy < zy if x < z and |str(x)| = |str(z)|
// xy < xw if y < w
// fold x < fold y if x < y
// fold x < nil
class BCCState_LL: public BCCState // {{{
{ public:
    std::vector<std::pair<RE*,int> > commonStack;
    std::vector<std::pair<RE*,int> > lhsStack;
    std::vector<std::pair<RE*,int> > rhsStack;
    std::vector<int> lhsStartIndex;
    std::vector<int> rhsStartIndex;
    unsigned int lhsIndex;
    unsigned int rhsIndex;
}; // }}}

/** BCComparer defines the function type of a resuming comparrison between
  * BitCodes.
  * Resuming means that the sub-result of the first part of the comparrison is
  * given as an argument, and only the (small) delta continuations of the lhs
  * and rhs have to be applied to the sub result in order to determine the
  * result of the comparrison.
  * BCLEQ implements the trivial order (always false)
  * BCLEQ_GL implements a greedy leftmost (corresponding to lexicographical) ordering)
  * BCLEQ_LL implements a lonest-leftmost ordering
  */
typedef bool (*BCComparer)(const BitCode &lhs_cont, const BitCode &rhs_cont, const BCCState &state);
/** BCUpdater updates the given sub-result with delta continuations for the lhs
  * and rhs to obtain a new sub-result that can be used as a staring point of a
  * BCComparer.
  * BCUPD implements updating for the trivial order (returns copy)
  * BCUPD_GL implements updating for greedy leftmost ordering
  * BCUPD_LL implements updating for longest-leftmost ordering
  */
typedef BCCState *(*BCUpdater)(const BitCode &lhs_cont, const BitCode &rhs_cont, const BCCState &state);

// Undefined disambiguation
bool BCLEQ_NN(const BitCode &lhs, const BitCode &rhs, const BCCState &state);
BCCState *BCUPD_NN(const BitCode &lhs, const BitCode &rhs, const BCCState &state);
// Greedy Leftmost disambiguation
bool BCLEQ_GL(const BitCode &lhs, const BitCode &rhs, const BCCState &state);
BCCState *BCUPD_GL(const BitCode &lhs, const BitCode &rhs, const BCCState &state);
// Longest Leftmost disambiguation
bool BCLEQ_LL(const BitCode &lhs, const BitCode &rhs, BCCState &state);
BCCState *BCUPD_LL(const BitCode &lhs, const BitCode &rhs, const BCCState &state);

/** BCCStates holds the commparrison state for each pair of nodes */
typedef std::map<int,std::map<int,BCCState*> > BCCStates;
#endif
