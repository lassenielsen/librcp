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

BCOrder_LL::BCOrder_LL(const RE &re) // {{{
{ myRE=re.Copy();
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
  { return bc.GetBit(pos)==BitCode::INL?
	   PartSerialize(bc,&(re_sum->GetLeft()),pos+1):
           PartSerialize(bc,&(re_sum->GetRight()),pos+1);
  }
  if (re_star!=NULL)
  { if (bc.GetBit(pos)==BitCode::CONS)
    { PS_RESULT result1=PartSerialize(bc,&(re_star->GetSub()),pos+1);
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
{ public:
    int result; // <0 => LESS, =0 => EQUAL >0 => GREATER
    int length1;
    int length2;
    int pos1;
    int pos2;
}; // }}}
LEQ_RESULT LEQ_LL(const BitCode &bc1, int pos1, // {{{
                  const BitCode &bc2, int pos2,
                  const RE *exp)
{ const RE_One *re_one = dynamic_cast<const RE_One*>(exp);
  const RE_Char *re_char = dynamic_cast<const RE_Char*>(exp);
  const RE_Seq *re_seq = dynamic_cast<const RE_Seq*>(exp);
  const RE_Sum *re_sum = dynamic_cast<const RE_Sum*>(exp);
  const RE_Star *re_star = dynamic_cast<const RE_Star*>(exp);
  if (re_one!=NULL)
  { LEQ_RESULT result;
    result.result=0;
    result.pos1=pos1;
    result.pos2=pos2;
    result.length1=0;
    result.length2=0;
    return result;
  }
  else if (re_char!=NULL)
  { LEQ_RESULT result;
    result.result=0;
    result.pos1=pos1;
    result.pos2=pos2;
    result.length1==1;
    result.length2==1;
    return result;
  }
  else if (re_seq!=NULL)
  { LEQ_RESULT result1=LEQ_LL(bc1,pos1,bc2,pos2,&(re_seq->GetFront()));
    LEQ_RESULT result2=LEQ_LL(bc1,result1.pos1,bc2,result1.pos2,&(re_seq->GetBack()));
    LEQ_RESULT result;
    if (result1.length1>result1.length2)
      result.result=-1;
    else if (result1.length1<result1.length2)
      result.result=1;
    else
      result.result=result1.result!=0?result1.result:result2.result;
    result.pos1=result2.pos1;
    result.pos2=result2.pos2;
    result.length1=result1.length1+result2.length1;
    result.length2=result1.length2+result2.length2;
    return result;
  }
  else if (re_sum!=NULL)
  { try
    { if ((bc1.GetBit(pos1)==BitCode::INL) && (bc2.GetBit(pos2)==BitCode::INL))
        return LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_sum->GetLeft()));
      else if ((bc1.GetBit(pos1)==BitCode::INR) && (bc2.GetBit(pos2)==BitCode::INR))
        return LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_sum->GetRight()));
      else
      { PS_RESULT ps1= PartSerialize(bc1,exp,pos1);
        PS_RESULT ps2= PartSerialize(bc2,exp,pos2);
        LEQ_RESULT result;
        if ((bc1.GetBit(pos1)==BitCode::INL) && (bc2.GetBit(pos2)==BitCode::INR))
          result.result=-1;
        else
          result.result=1;
        result.pos1=ps1.pos;
        result.pos2=ps2.pos;
        result.length1=ps1.length;
        result.length2=ps2.length;
        return result;
      }
    }
    catch (string s)
    { LEQ_RESULT result;
      result.result=0;
      result.pos1=pos1;
      result.pos2=pos2;
      result.length1=0;
      result.length2=0;
      return result;
    }
  }
  else if (re_star!=NULL)
  { try
    { if ((bc1.GetBit(pos1)==BitCode::CONS) && (bc2.GetBit(pos2)==BitCode::CONS))
      { LEQ_RESULT result1=LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_star->GetSub()));
        LEQ_RESULT result2=LEQ_LL(bc1,result1.pos1,bc2,result1.pos2,exp);
        LEQ_RESULT result;
        if (result1.length1>result1.length2)
          result.result=-1;
        else if (result1.length1<result1.length2)
          result.result=1;
        else
          result.result=result1.result!=0?result1.result:result2.result;
        result.pos1=result2.pos1;
        result.pos2=result2.pos2;
        result.length1=result1.length1+result2.length1;
        result.length2=result1.length2+result2.length2;
        return result;
      }
      else if ((bc1.GetBit(pos1)==BitCode::NIL) && (bc2.GetBit(pos2)==BitCode::NIL))
      { LEQ_RESULT result;
        result.result=0;
        result.pos1=pos1+1;
        result.pos2=pos2+1;
        result.length1=0;
        result.length2=0;
        return result;
      }
      else
      { PS_RESULT ps1= PartSerialize(bc1,exp,pos1);
        PS_RESULT ps2= PartSerialize(bc2,exp,pos2);
        LEQ_RESULT result;
        if ((bc1.GetBit(pos1)==BitCode::CONS) && (bc2.GetBit(pos2)==BitCode::NIL))
          result.result=-1;
        else
          result.result=1;
        result.pos1=ps1.pos;
        result.pos2=ps2.pos;
        result.length1=ps1.length;
        result.length2=ps2.length;
        return result;
      }
    }
    catch (string s)
    { LEQ_RESULT result;
      result.result=0;
      result.pos1=pos1;
      result.pos2=pos2;
      result.length1=0;
      result.length2=0;
      return result;
    }
  }
  throw (string)"Error: Bad expression";
} // }}}
bool BCOrder_LL::LEQ(const BitCode &lhs, const BitCode &rhs) const // {{{
{ LEQ_RESULT result = LEQ_LL(lhs,0,rhs,0,myRE);
  return result.result<=0;
} // }}}

