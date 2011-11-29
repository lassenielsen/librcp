#include <rcp/bitcode_order.hpp>
using namespace std;

BCOrder::BCOrder() // {{{
{
} // }}}
BCOrder::~BCOrder() // {{{
{
} // }}}
bool BCOrder::LEQ(const BitCode &lhs, const BitCode &rhs) const // {{{
{ return false;
} // }}}

BCOrder_GL::BCOrder_GL() // {{{
{
} // }}}
BCOrder_GL::~BCOrder_GL() // {{{
{
} // }}}
bool BCOrder_GL::LEQ(const BitCode &lhs, const BitCode &rhs) const // {{{
{ for (int i=0; i<lhs.GetLength(); ++i)
  { if (i>=rhs.GetLength())
      return false;
    if (lhs.GetBit(i) && !rhs.GetBit(i))
      return false;
    if (rhs.GetBit(i) && !lhs.GetBit(i))
      return true;
  }
  return true;
} // }}}

BCOrder_LL::BCOrder_LL(const RE *re) // {{{
{ myRE=re->Copy();
} // }}}
BCOrder_LL::~BCOrder_LL() // {{{
{ delete myRE;
} // }}}
class PS_RESULT // {{{
{ public:
    int length;
    int pos;
}; // }}}
PS_RESULT PartSerialize(const BitCode &bc, const RE *exp, int pos) // {{{
{ const RE_One *re_one = dynamic_cast<const RE_One*>(exp);
  const RE_Char *re_char = dynamic_cast<const RE_Char*>(exp);
  const RE_Seq *re_seq = dynamic_cast<const RE_Seq*>(exp);
  const RE_Sum *re_sum = dynamic_cast<const RE_Sum*>(exp);
  const RE_Star *re_star = dynamic_cast<const RE_Star*>(exp);
  if (re_one!=NULL)
  { PS_RESULT result;
    result.pos=pos;
    result.length=0;
    return result;
  }
  if (re_char!=NULL)
  { PS_RESULT result;
    result.pos=pos;
    result.length=1;
    return result;
  }
  if (re_seq!=NULL)
  { PS_RESULT result1=PartSerialize(bc,&(re_seq->GetFront()),pos);
    int midpos=result1.pos;
    PS_RESULT result2=PartSerialize(bc,&(re_seq->GetBack()),midpos);
    PS_RESULT result;
    result.pos=result2.pos;
    result.length=result1.length+ result2.length;
    return result;
  }
  if (re_sum!=NULL)
  { return bc.GetBit(pos)?
           PartSerialize(bc,&(re_sum->GetRight()),pos+1):
	   PartSerialize(bc,&(re_sum->GetLeft()),pos+1);
  }
  if (re_star!=NULL)
  { if (bc.GetBit(pos))
    { PS_RESULT result1=PartSerialize(bc,&(re_star->GetSub()),pos);
      int midpos=result1.pos;
      PS_RESULT result2=PartSerialize(bc,re_star,midpos);
      PS_RESULT result;
      result.pos=result2.pos;
      result.length=result1.length+result2.length;
      return result;
    }
    else
    { PS_RESULT result;
      result.pos=pos+1;
      result.length=0;
      return result;
    }
  }
} // }}}
class LEQ_RESULT // {{{
{ bool result;
  int length1;
  int length2;
  int pos1;
  int pos2;
}; // }}}
bool BCOrder_LL::LEQ(const BitCode &lhs, const BitCode &rhs) const // {{{
{ for (int i=0; i<lhs.GetLength(); ++i)
  { if (i>=rhs.GetLength())
      return false;
    if (lhs.GetBit(i) && !rhs.GetBit(i))
      return false;
    if (rhs.GetBit(i) && !lhs.GetBit(i))
      return true;
  }
  return true;
} // }}}

