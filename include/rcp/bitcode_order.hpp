#ifndef RCP_BCORDER
#define RCP_BCORDER

#include <vector>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <rcp/re.hpp>

class BCCState // {{{
{ public:
    BCCState();
    virtual ~BCCState();
    virtual std::string ToString() const;
}; // }}}
class BCCState_GL : public BCCState // {{{
{ public:
    BCCState_GL();
    virtual ~BCCState_GL();
    int state; // -1=less, 0=equal, 1=greater
    BitCode lhsBuffer, rhsBuffer;
    std::string ToString() const;
}; // }}}
class LL_Trace // Represents an (un)completed stub {{{
{ public:
    LL_Trace(unsigned int index, const RE* exp);
    virtual ~LL_Trace();
    virtual bool LEQ() const;
    virtual std::string ToString() const;

    unsigned int myEntryIndex;
    unsigned int myIndex;
    int myStatus; //-1=less, 0=equal, 1=greater
    const RE *myExp;
    bool myCompleted;
}; // }}}
class LL_Trace_Sub : public LL_Trace // Represents traces with (first level) similar subtraces {{{
{ public:
    LL_Trace_Sub();
    virtual ~LL_Trace_Sub();
    bool LEQ() const;
    std::string ToString() const;

  private:
    std::string myStep; // left, right, first, snd, cons
    LL_Trace *mySub;
}; // }}}
class LL_Trace_Split : public LL_Trace // Respresents traces with dissimilar subtraces {{{
{ public:
    LL_Trace_Split();
    virtual ~LL_Trace_Split();
    bool LEQ() const;
    std::string ToString() const;

    LL_Trace *myTail;
    LL_Trace *myLeft;
    LL_Trace *myRight;
}; // }}}
class BCCState_LL : public BCCState // {{{
{ public:
    BCCState_GL();
    virtual ~BCCState_GL();
    LL_Trace *trace;
    std::string ToString() const;
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
//bool BCLEQ_LL(const BitCode &lhs, const BitCode &rhs, BCCState &state);
//BCCState *BCUPD_LL(const BitCode &lhs, const BitCode &rhs, const BCCState &state);

/** BCCStates holds the commparrison state for each pair of nodes */
typedef std::map<int,std::map<int,BCCState*> > BCCStates;
#endif
