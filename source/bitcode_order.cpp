#include <rcp/bitcode_order.hpp>
#include <iostream>
#include <sstream>
using namespace std;

/***********************************************
 ** Implementation of BCCState and subclasses **
 ***********************************************/
BCCState::BCCState() // {{{
{
} // }}}
BCCState::~BCCState() // {{{
{
} // }}}
string BCCState::ToString() const // {{{
{ return "BCCSTATE";
} // }}}

BCCState_GL::BCCState_GL() // {{{
: state(0)
{
} // }}}
BCCState_GL::~BCCState_GL() // {{{
{
} // }}}
string BCCState_GL::ToString() const // {{{
{ stringstream ss;
  ss << "BCCSTATE_GL(" << state << "," << lhsBuffer.ToString() << "," << rhsBuffer.ToString() << ")";
return ss.str();
} // }}}

bool BCLEQ_NN(const BitCode &lhs, const BitCode &rhs, const BCCState &state) // {{{
{ return false;
} // }}}
BCCState *BCUPD_NN(const BitCode &lhs, const BitCode &rhs, const BCCState &state) // {{{
{ return new BCCState();
} // }}}

bool BCLEQ_GL(const BitCode &lhs, const BitCode &rhs, const BCCState &given_state) // {{{
{ const BCCState_GL *state = dynamic_cast<const BCCState_GL*>(&given_state);
  if (state==NULL)
    throw (string)"BCLEQ_GL bad state exception";

  if (state->state<0)
    return true;
  if (state->state>0)
    return false;

  BitCode lhsBits=state->lhsBuffer;
  lhsBits.Append(lhs);
  BitCode rhsBits=state->rhsBuffer;
  rhsBits.Append(rhs);
  for (int i=0; i<lhsBits.GetLength() && i<rhsBits.GetLength(); ++i)
  { if (lhsBits.GetBit(i) && !rhsBits.GetBit(i))
      return false;
    if (!lhsBits.GetBit(i) && rhsBits.GetBit(i))
      return true;
  }
  return !(rhsBits.GetLength()<lhsBits.GetLength());
} // }}}
BCCState *BCUPD_GL(const BitCode &lhs, const BitCode &rhs, const BCCState &given_state) // {{{
{ const BCCState_GL *state = dynamic_cast<const BCCState_GL*>(&given_state);
  if (state==NULL)
    throw (string)"BCUPD_GL bad state exception";

  BCCState_GL *result = new BCCState_GL();
  result->state=state->state;

  if (state->state<0)
    return result;
  if (state->state>0)
    return result;

  BitCode lhsBits=state->lhsBuffer;
  lhsBits.Append(lhs);
  result->lhsBuffer=BitCode(); // clear buffer
  BitCode rhsBits=state->rhsBuffer;
  rhsBits.Append(rhs);
  result->rhsBuffer=BitCode(); // clear buffer
  for (int i=0; i<lhsBits.GetLength() && i<rhsBits.GetLength(); ++i)
  { if (lhsBits.GetBit(i) && !rhsBits.GetBit(i))
    { result->state=1;
      return result;
    }
    if (!lhsBits.GetBit(i) && rhsBits.GetBit(i))
    { result->state=-1;
      return result;
    }
  }
  // Set buffers before returning
  for (int i=rhsBits.GetLength();i<lhsBits.GetLength(); ++i)
    result->lhsBuffer.PushBit(lhsBits.GetBit(i));
  for (int i=lhsBits.GetLength();i<rhsBits.GetLength(); ++i)
    result->rhsBuffer.PushBit(rhsBits.GetBit(i));
  return result;
} // }}}

class PS_RESULT // {{{
{ public:
    int length;
    int pos;
    bool cyclic;
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
    result.cyclic=false;
    return result;
  }
  if (re_char!=NULL)
  { PS_RESULT result;
    result.pos=pos;
    result.length=1;
    result.cyclic=false;
    return result;
  }
  if (re_seq!=NULL)
  { PS_RESULT result1=PartSerialize(bc,&(re_seq->GetFront()),pos);
    int midpos=result1.pos;
    PS_RESULT result2=PartSerialize(bc,&(re_seq->GetBack()),midpos);
    PS_RESULT result;
    result.pos=result2.pos;
    result.length=result1.length+ result2.length;
    result.cyclic=result1.cyclic || result2.cyclic;
    return result;
  }
  if (re_sum!=NULL)
  { try
    { return bc.GetBit(pos)==BitCode::INL?
             PartSerialize(bc,&(re_sum->GetLeft()),pos+1):
             PartSerialize(bc,&(re_sum->GetRight()),pos+1);
    }
    catch (string s)
    { PS_RESULT result;
      result.pos=pos;
      result.length=0;
      result.cyclic=0;
      return result;
    }
  }
  if (re_star!=NULL)
  { try
    { if (bc.GetBit(pos)==BitCode::CONS)
      { PS_RESULT result1=PartSerialize(bc,&(re_star->GetSub()),pos+1);
        int midpos=result1.pos;
        PS_RESULT result2=PartSerialize(bc,re_star,midpos);
        PS_RESULT result;
        result.pos=result2.pos;
        result.length=result1.length+result2.length;
        result.cyclic=result1.cyclic || result2.cyclic || result1.length==0;
        return result;
      }
      else
      { PS_RESULT result;
        result.pos=pos+1;
        result.length=0;
        result.cyclic=false;
        return result;
      }
    }
    catch (string s)
    { PS_RESULT result;
      result.pos=pos;
      result.length=0;
      result.cyclic=0;
      return result;
    }
  }
  throw string("PartSerialize: Unknown construct");
} // }}}
class LEQ_RESULT // {{{
{ public:
    int result; // <0 => LESS, =0 => EQUAL >0 => GREATER
    int length1;
    int length2;
    int pos1;
    int pos2;
}; // }}}
////LEQ_RESULT BCLEQ_LL1(const BitCode &bc1, int pos1, // {{{
////                     const BitCode &bc2, int pos2,
////                     vector<pair<RE*,int> > &stack)
////{ 
////  while (stack.size()>0)
////  {
////    RE *exp=stack.back();
////    const RE_One *re_one = dynamic_cast<const RE_One*>(exp);
////    const RE_Char *re_char = dynamic_cast<const RE_Char*>(exp);
////    const RE_Seq *re_seq = dynamic_cast<const RE_Seq*>(exp);
////    const RE_Sum *re_sum = dynamic_cast<const RE_Sum*>(exp);
////    const RE_Star *re_star = dynamic_cast<const RE_Star*>(exp);
////    if (re_one!=NULL)
////    { LEQ_RESULT result;
////      result.result=0;
////      result.pos1=pos1;
////      result.pos2=pos2;
////      result.length1=0;
////      result.length2=0;
////      return result;
////    }
////    else if (re_char!=NULL)
////    { LEQ_RESULT result;
////      result.result=0;
////      result.pos1=pos1;
////      result.pos2=pos2;
////      result.length1==1;
////      result.length2==1;
////      return result;
////    }
////    else if (re_seq!=NULL)
////    { stack.pop_back();
////      stack.push_back(&(re_seq->GetFront()));
////      LEQ_RESULT result1=BCLEQ_LL1(bc1,pos1,bc2,pos2,stack);
////      stack.pop_back();
////      stack.push_back(&(re_seq->GetBack()));
////      LEQ_RESULT result2=BCLEQ_LL2(bc1,result1.pos1,bc2,result1.pos2,stack);
////      LEQ_RESULT result;
////      stack.pop_back();
////      stack.push_back(exp);
////      if (result1.length1>result1.length2)
////        result.result=-1;
////      else if (result1.length1<result1.length2)
////        result.result=1;
////      else
////        result.result=result1.result!=0?result1.result:result2.result;
////      result.pos1=result2.pos1;
////      result.pos2=result2.pos2;
////      result.length1=result1.length1+result2.length1;
////      result.length2=result1.length2+result2.length2;
////      return result;
////    }
////    else if (re_sum!=NULL)
////    { try
////      { if ((bc1.GetBit(pos1)==BitCode::INL) && (bc2.GetBit(pos2)==BitCode::INL))
////          return LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_sum->GetLeft()));
////        else if ((bc1.GetBit(pos1)==BitCode::INR) && (bc2.GetBit(pos2)==BitCode::INR))
////          return LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_sum->GetRight()));
////        else
////        { PS_RESULT ps1= PartSerialize(bc1,exp,pos1);
////          PS_RESULT ps2= PartSerialize(bc2,exp,pos2);
////          LEQ_RESULT result;
////          if ((bc1.GetBit(pos1)==BitCode::INL) && (bc2.GetBit(pos2)==BitCode::INR))
////            result.result=-1;
////          else
////            result.result=1;
////          result.pos1=ps1.pos;
////          result.pos2=ps2.pos;
////          result.length1=ps1.length;
////          result.length2=ps2.length;
////          return result;
////        }
////      }
////      catch (string s)
////      { LEQ_RESULT result;
////        result.result=0;
////        result.pos1=pos1;
////        result.pos2=pos2;
////        result.length1=0;
////        result.length2=0;
////        return result;
////      }
////    }
////    else if (re_star!=NULL)
////    { try
////      { if ((bc1.GetBit(pos1)==BitCode::CONS) && (bc2.GetBit(pos2)==BitCode::CONS))
////        { LEQ_RESULT result1=LEQ_LL(bc1,pos1+1,bc2,pos2+1,&(re_star->GetSub()));
////          LEQ_RESULT result2=LEQ_LL(bc1,result1.pos1,bc2,result1.pos2,exp);
////          LEQ_RESULT result;
////          if (result1.length1>result1.length2)
////            result.result=-1;
////          else if (result1.length1<result1.length2)
////            result.result=1;
////          else
////            result.result=result1.result!=0?result1.result:result2.result;
////          result.pos1=result2.pos1;
////          result.pos2=result2.pos2;
////          result.length1=result1.length1+result2.length1;
////          result.length2=result1.length2+result2.length2;
////          return result;
////        }
////        else if ((bc1.GetBit(pos1)==BitCode::NIL) && (bc2.GetBit(pos2)==BitCode::NIL))
////        { LEQ_RESULT result;
////          result.result=0;
////          result.pos1=pos1+1;
////          result.pos2=pos2+1;
////          result.length1=0;
////          result.length2=0;
////          return result;
////        }
////        else
////        { PS_RESULT ps1= PartSerialize(bc1,exp,pos1);
////          PS_RESULT ps2= PartSerialize(bc2,exp,pos2);
////          LEQ_RESULT result;
////          if ((bc1.GetBit(pos1)==BitCode::CONS) && (bc2.GetBit(pos2)==BitCode::NIL))
////            result.result=-1;
////          else
////            result.result=1;
////          result.pos1=ps1.pos;
////          result.pos2=ps2.pos;
////          result.length1=ps1.length;
////          result.length2=ps2.length;
////          return result;
////        }
////      }
////      catch (string s)
////      { LEQ_RESULT result;
////        result.result=0;
////        result.pos1=pos1;
////        result.pos2=pos2;
////        result.length1=0;
////        result.length2=0;
////        return result;
////      }
////    }
////    throw (string)"Error: Bad expression";
////  }
////} // }}}
////bool BCLEQ_LL::LEQ(const BitCode &lhs, const BitCode &rhs, int &pos, vector<pair<RE*,int> > &stack) const // {{{
////{ if (stack.size()<=0)
////    throw (string)"BCLEQ_LL called with empty stack!"
////  pair<RE*,int> state=stack.back();
////  PS_RESULT ps1=PartSerialize(lhs,state,pos);
////  PS_RESULT ps2=PartSerialize(rhs,state,pos);
////  if (ps2.cyclic && !ps1.cyclic)
////    return true;
////  else if (ps1.cyclic && !ps2.cyclic)
////    return false;
////  else
////  { LEQ_RESULT result = BCLEQ_LL1(lhs,pos,rhs,pos,stack);
////    if (result.result<=0)
////    { pos=result.pos1;
////      return true;
////    }
////    else
////    { pos=result.pos2;
////      return false;
////    }
////  }
////} // }}}
