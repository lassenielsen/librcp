#ifndef RCP_BCORDER
#define RCP_BCORDER

#include <vector>
#include <string>
#include <rcp/re.hpp>

/** BCOrder defines an ordering of bitcodes
  * BCOrder implements the trivial order (always false)
  * but subclasses implement orders for greedy-leftmost and longest-leftmost
  */
class BCOrder // {{{
{ public:
    BCOrder();
    virtual ~BCOrder();

    virtual bool LEQ(const BitCode &lhs, const BitCode &rhs) const;
}; // }}}
/** Greedy-Leftmost bitcode ordering
  */
class BCOrder_GL : public BCOrder // {{{
{ public:
    BCOrder_GL();
    virtual ~BCOrder_GL();

    bool LEQ(const BitCode &lhs, const BitCode &rhs) const;
}; // }}}
/** Longest-Leftmost bitcode ordering
  */
class BCOrder_LL : public BCOrder // {{{
{ public:
    BCOrder_LL(const RE &re);
    virtual ~BCOrder_LL();

    bool LEQ(const BitCode &lhs, const BitCode &rhs) const;

  private:
    RE *myRE;
}; // }}}

#endif

