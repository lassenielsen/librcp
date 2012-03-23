#ifndef RCP_BCORDER
#define RCP_BCORDER

#include <vector>
#include <utility>
#include <rcp/re.hpp>

class BCCState // {{{
{ public:
    BCCState();
    virtual ~BCCState();
    virtual BCCState *Copy() const;

  private:
}; // }}}
class BCCState_GL : public BCCState // {{{
{ public:
    BCCState_GL(unsigned int pos);
    virtual ~BCCState_GL();
    unsigned int GetPosition() const;
    void SetPosition(unsigned int newPos);

    virtual BCCState_GL *Copy() const;

  private:
    unsigned int myPosition;
}; // }}}
class BCCCallState_LL // {{{
{ public:
    RE *myRE;  // The sub expression at the current position
    int state; // Represents the state of the current subexpression (eg, did we call to the left or right sub-expression)
}; // }}}
//class BCCState_LL : public BCCState_GL // {{{
//{ public:
//    BCCState_LL(unsigned int pos, std::vector<BCCCallState_LL>);
//    virtual ~BCCState_LL();
//
//    virtual BCCState_GL *Copy() const;
//
//  private:
//    unsigned int myPosition;
//    std::vector<BCCCallState_LL> myCallStack;
//}; // }}}

/** BCComparer defines the function type of a resumable comparrison between
  * BitCodes.
  * Resumable means that when the comparrison is determined the current
  * comparrison state is returned, that can be used to resume the comparrison
  * of two bitcodes both having the determined least bitcode as a prefix.
  * BCLEQ implements the trivial order (always false)
  * BCLEQ_GL implements a greedy leftmost (corresponding to lexicographical) ordering)
  * BCLEQ_LL implements a lonest-leftmost ordering
  */
typedef BCCState *(*BCCInitState)(RE *re);
typedef bool (*BCComparer)(const BitCode &lhs, const BitCode &rhs, BCCState *state);

// Undefined disambiguation
BCCState *InitState_NN(RE *re);
bool BCLEQ_NN(const BitCode &lhs, const BitCode &rhs, BCCState *state);

// Greedy Leftmost disambiguation
BCCState *InitState_GL(RE *re);
bool BCLEQ_GL(const BitCode &lhs, const BitCode &rhs, BCCState *state);

// Longest Leftmost disambiguation
//BCCState *InitState_LL(RE *re);
//bool BCLEQ_LL(const BitCode &lhs, const BitCode &rhs, BCCState *state);

/** Define the state of comparison memoization with usefull functionality
  */
class BCCStates // {{{
{
  public:
    BCCStates(const BCCState &init_state, int nodes);
    ~BCCStates();

    void SetState(int x, int y, const BCCState &new_state);
    void ShiftState(int source, int dest);
    BCCState *GetState(int x, int y);

  private:
    std::map<int,std::map<int,BCCState*> > myStates;
    int mySize;
}; // }}}
#endif
