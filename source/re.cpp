#include <rcp/common.hpp>
#include <rcp/re.hpp>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>

using namespace std;

// Implementation of RE Parsing
// A parseelt is one of the following types:
// A fully parsed regular expression
// A a group containing a list of ParseElts
// A left parenthesis
// A right parenthesis
// A star
// A plus
enum elttype {isGroup,isExp,isLpar,isRpar,isStar,isPlus,isOption,isInterval};
// Implementation of datatype with constructors from elttype
class ParseElt // {{{
{ public:
    // Construct group element
    ParseElt(list<ParseElt*> &group);
    // Construct RE element
    ParseElt(RE *exp);
    // Construct Interval element
    ParseElt(int min, int max);
    // Construct simple element
    ParseElt(elttype type);
    // Construct copy
    ParseElt(const ParseElt &rhs);
    virtual ~ParseElt();
    ParseElt &operator=(const ParseElt &rhs);

    // Get the conained RE
    const RE &GetExp() const;
    RE &GetExp();
    RE *ReleaseExp(); // Release RE, and avoid copying
    // Get the conained group
    const list<ParseElt*> &GetGroup() const;
    list<ParseElt*> &GetGroup();

    // Interval accessors
    int GetMin() const;
    int GetMax() const;

    // Get type of element
    elttype GetType() const;

  private:
    RE *myExp;
    elttype myType;
    list<ParseElt*> myGroup;
    int myMin;
    int myMax;
}; // }}}

ParseElt::ParseElt(list<ParseElt*> &group) // {{{
{ myType=isGroup;
  myGroup=group;
} // }}}
ParseElt::ParseElt(RE *exp) // {{{
{ myType=isExp;
  myExp=exp;
} // }}}
ParseElt::ParseElt(elttype type) // {{{
{ myType=type;
  if (myType==isExp)
    myExp=new RE();
} // }}}
ParseElt::ParseElt(int min, int max) // {{{
{ myType=isInterval;
  myMin=min;
  myMax=max;
} // }}}
ParseElt::ParseElt(const ParseElt &rhs) // {{{
{ myType=rhs.GetType();
  if (myType==isExp)
    myExp=rhs.GetExp().Copy();
  else if (myType==isGroup)
    CopyList<ParseElt>(rhs.GetGroup(),myGroup);
} // }}}
ParseElt &ParseElt::operator=(const ParseElt &rhs) // {{{
{ if (myType==isExp)
    delete myExp;
  if (myType==isGroup)
    DeleteList(myGroup);

  myType=rhs.GetType();
  if (myType==isExp)
    myExp=rhs.GetExp().Copy();
  else if (myType==isGroup)
    CopyList(rhs.GetGroup(),myGroup);

  return *this;
} // }}}
ParseElt::~ParseElt() // {{{
{ if (myType==isExp)
    delete myExp;
  else if (myType==isGroup)
    DeleteList(myGroup);
} // }}}

const RE &ParseElt::GetExp() const // {{{
{ if (myType != isExp)
    throw "GetExp called on non Exp ParseElt";
  return *myExp;
} // }}}
RE &ParseElt::GetExp()// {{{
{ if (myType != isExp)
    throw "GetExp called on non Exp ParseElt";
  return *myExp;
} // }}}
RE *ParseElt::ReleaseExp()// {{{
{ if (myType != isExp)
    throw "ReleaseExp called on non Exp ParseElt";
  RE *result = myExp;
  myExp = new RE();
  return result;
} // }}}
const list<ParseElt*> &ParseElt::GetGroup() const // {{{
{ if (myType != isGroup)
    throw "GetGroup called on non Group ParseElt";
  return myGroup;
} // }}}
list<ParseElt*> &ParseElt::GetGroup() // {{{
{ if (myType != isGroup)
    throw "GetGroup called on non Group ParseElt";
  return myGroup;
} // }}}
int ParseElt::GetMin() const // {{{
{ if (myType != isInterval)
    throw "GetMin called on non Interval ParseElt";
  return myMin;
} // }}}
int ParseElt::GetMax() const // {{{
{ if (myType != isInterval)
    throw "GetMax called on non Interval ParseElt";
  return myMax;
} // }}}
elttype ParseElt::GetType() const // {{{
{ return myType;
} // }}}

vector<char> makeRange(int &pos, const std::string &s) // {{{
{ vector<char> result;
  bool invert=false;
  if (s[pos]=='^')
  { invert=true;
    ++pos;
  }
  bool done=false;
  while (not done)
  { if (pos==s.size())
      throw "makeRange: range exceeds end of string";
    switch (s[pos])
    { case ']':
        done=true;
        break;
      case '\\':
        if (pos+1==s.size())
          throw "makeRange: ends with unescaped backslash";
        else
          result.push_back(s[pos+1]);
        pos +=2;
        break;
      default:
        if (s.size()>pos+2 && s[pos+1]=='-' && s[pos+2]!=']')
        { for (int ch=s[pos]; ch<=s[pos+2]; ++ch)
            result.push_back((char)ch);
          pos+=3;
        }
        else
        { result.push_back(s[pos]);
          ++pos;
        }
    }
  }
  if (invert)
  { vector<char> inverted=result;
    result.clear();
    for (int ch=0; ch<256; ++ch)
      if (find(inverted.begin(), inverted.end(), (char)ch)==inverted.end()) // ch not in inverted
        result.push_back((char)ch);
  }
  return result;
} // }}}
RE *makeRangeExp(vector<char> &elements, int start=0, int end=-1) // {{{
{ if (end<0)
    end=elements.size()-1;
  if (end<start) // empty range
    return new RE();
  else if (start==end) // single element
    return new RE_Char(elements[start]);
  else
  { int mid = (start+end)/2;
    RE *lhs=makeRangeExp(elements,start,mid);
    RE *rhs=makeRangeExp(elements,mid+1,end);
    return new RE_Sum(lhs,rhs);
  }
} // }}}
ParseElt *makeInterval(int &pos, const std::string &s) // {{{
{ int min=0;
  int max=0;
  bool done=false;
  while (not done)
  { if (pos==s.size())
      throw "makeInterval: no , after {";
    if (s[pos]>='0' && s[pos]<='9')
    { min*=10;
      min+=s[pos]-'0';
    }
    else if (s[pos]==',')
      done=true;
    else
      throw "makeInterval: syntax error before ,";
    ++pos;
  }
  done=false;
  while (not done)
  { if (pos==s.size())
      throw "makeInterval: no } after ,";
    if (s[pos]>='0' && s[pos]<='9')
    { max*=10;
      max+=s[pos]-'0';
      ++pos;
    }
    else if (s[pos]=='}')
      done=true;
    else
      throw "makeInterval: syntax error before }";
  }
  return new ParseElt(min,max);
} // }}}
RE *makeIntervalExp(RE *exp,int min, int max) // {{{
{ if (max<min || max<0)
  { delete exp;
    return new RE();
  }
  else if (min==0 && max==0)
  { delete exp;
    return new RE_One();
  }
  else if (min==1 && max==1)
  { return exp;
  }
  else if (min==0 && max==1)
  { return new RE_Sum(new RE_One(),exp);
  }
  else if (min<=0)
  { return new RE_Sum(new RE_One(), new RE_Seq(exp->Copy(),makeIntervalExp(exp,0,max-1)));
  }
  else
    return new RE_Seq(exp->Copy(),makeIntervalExp(exp,min-1,max-1));
} // }}}

// Turn string into list of tokens (tokenize)
void makeReg1(const string &def, list<ParseElt*> &dest) // Tokenize {{{
{ for (int i=0; i<def.size(); ++i)
  { switch (def[i])
    { case '\\':
      { if (i+1==def.size())
          throw "makeReg1: def ends with unescaped backslash!";
        ParseElt *elt = new ParseElt(new RE_Char(def[++i])); // Add escaped char to result
        dest.push_back(elt);
        break;
      }
      case '[':
      { ++i;
        vector<char> range=makeRange(i,def);
        ParseElt *elt = new ParseElt(makeRangeExp(range)); // Make exp of alternations between elements
        dest.push_back(elt);
        break;
      }
      case '{':
      { ++i;
        ParseElt *elt = makeInterval(i,def);
        dest.push_back(elt);
        break;
      }
      case '(':
      { ParseElt *elt = new ParseElt(isLpar); // Add left parenthesis to result
        dest.push_back(elt);
        break;
      }
      case ')':
      { ParseElt *elt = new ParseElt(isRpar); // Add right parenthesis to result
        dest.push_back(elt);
        break;
      }
      case '+':
      { ParseElt *elt = new ParseElt(isPlus); // Add sum token to result
        dest.push_back(elt);
        break;
      }
      case '*':
      { ParseElt *elt = new ParseElt(isStar); // Add star token to result
        dest.push_back(elt);
        break;
      }
      case '?':
      { ParseElt *elt = new ParseElt(isOption); // Add option token to result
        dest.push_back(elt);
        break;
      }
      case '0':
      { ParseElt *elt = new ParseElt(new RE()); // Add exp token for 0 to result
        dest.push_back(elt);
        break;
      }
      case '1':
      { ParseElt *elt = new ParseElt(new RE_One()); // Add exp token for 1 to result
        dest.push_back(elt);
        break;
      }
      default:
      { ParseElt *elt = new ParseElt(new RE_Char(def[i])); // Add char token to result
        dest.push_back(elt);
        break;
      }
    }
  }
} // }}}
// Group tokens by matching Lpar and Rpar.
// The result is cleansed from all Lpar and Rpar tokens, but contains Group tokens
void makeReg2(list<ParseElt*> &source, list<ParseElt*> &dest) // Create groups using parenthesis {{{
{ while (source.size()>0)
  {
    ParseElt *elt = source.front();
    source.pop_front();
    switch (elt->GetType())
    { case isRpar:
      { 
        delete elt;
        return; // FIXME: report missing RPars
        break;
      }
      case isLpar:
      { 
        delete elt;
        list<ParseElt*> group;
        makeReg2(source,group);
        dest.push_back(new ParseElt(group));
        break;
      }
      default:
      { dest.push_back(elt);
        break;
      }
    }
  }
} // }}}
// Declaration necessary for recursion.
// makeReg3 generates a regular expression
// from a list of tokens (without Lpar or Rpar tokens)
RE *makeReg3(list<ParseElt*>&);
// makeRegFlat calls makeReg3 on all groups, to turn them into Exp tokens
// Thus the result has no group elements.
void makeRegFlat(list<ParseElt*> &source, list<ParseElt*> &dest) // Make group elements into exp elements by recursion {{{
{ while (source.size()>0)
  {
    ParseElt *elt = source.front();
    source.pop_front();

    switch (elt->GetType())
    { case isGroup:
      { RE *re=makeReg3(elt->GetGroup());
        delete elt;
        dest.push_back(new ParseElt(re));
        break;
      }
      default:
      { dest.push_back(elt);
        break;
      }
    }
  }
} // }}}
// makeRegStar allpies all star tokens
// Thus the result has no star tokens
void makeRegStar(list<ParseElt*> &source, list<ParseElt*> &dest) // Apply star tokens {{{
{ // If source starts with r,*,... then combine them to r*,...
  // If source starts with r,?,... then combine them to 1+r,...
  // Otherwise move first element from source to dest
  while (source.size()>1)
  {
    ParseElt *elt1 = source.front();
    source.pop_front();
    ParseElt *elt2 = source.front();
    source.pop_front();

    if (elt1->GetType()==isExp &&
        elt2->GetType()==isStar) // source =r,*,...
    { RE *newExp = new RE_Star(elt1->ReleaseExp());
      delete elt1;
      delete elt2;
      source.push_front(new ParseElt(newExp)); // Make source=r*,...
    }
    else if (elt1->GetType()==isExp &&
             elt2->GetType()==isOption) // source =r,?,...
    { RE *newExp = new RE_Sum(new RE_One(),elt1->ReleaseExp());
      delete elt1;
      delete elt2;
      source.push_front(new ParseElt(newExp)); // Make source=1+r,...
    }
    else if (elt1->GetType()==isExp &&
             elt2->GetType()==isInterval) // source =r,{n,m},...
    { RE *newExp = makeIntervalExp(elt1->ReleaseExp(),elt2->GetMin(),elt2->GetMax());
      delete elt1;
      delete elt2;
      source.push_front(new ParseElt(newExp)); // Make source=rrr(1+r(1+r))
    }
    else
    { dest.push_back(elt1);
      source.push_front(elt2);
    }
  }

  // Move remaining element (if any) to dest
  while (source.size()>0)
  { ParseElt *elt = source.front();
    source.pop_front();
    dest.push_back(elt);
  }
} // }}}
// makeRegSeq collects all sequences of Exp tokens
// Thus the result is an altenating sequence of Exps and plus'es
void makeRegSeq(list<ParseElt*> &source, list<ParseElt*> &dest) // Apply plus tokens {{{
{ // If source starts with r1,r2,tail then reduces to r1.r2
  // Otherwise move first element from source to dest
  while (source.size()>1)
  {
    ParseElt *elt1 = source.front();
    source.pop_front();
    if (elt1->GetType()==isExp)
    {
      ParseElt *elt2 = source.front();
      source.pop_front();
      if (elt2->GetType()==isExp) // source =r1,r2,...
      {
        source.push_front(new ParseElt(new RE_Seq(elt1->ReleaseExp(),elt2->ReleaseExp())));
        delete elt1;
        delete elt2;
      }
      else
      { dest.push_back(elt1);
        dest.push_back(elt2);
      }
    }
    else
      dest.push_back(elt1);
  }

  // Move remaining elements (if any) to dest
  while (source.size()>0)
  { dest.push_back(source.front());
    source.pop_front();
  }
} // }}}
// makeRegPlus collect an altenating sequence of Exps and plus'es into a single Exp
RE *makeRegPlus(list<ParseElt*> &tokens) // Apply plus tokens {{{
{ // Source is now of the form r1,+,r2,+,...,rn
  // Collapse to r1+(r2+(...rn)...)
  if (tokens.size()==0)
    throw "makeRegPlus: Empty sum";

  ParseElt *elt = tokens.front();

  if (elt->GetType()!=isExp)
    throw "makeRegPlus: Non expression element";

  tokens.pop_front();

  RE *re=elt->ReleaseExp();
  delete elt;

  while (tokens.size()>0)
  { elt=tokens.front();
    if (elt->GetType()!=isPlus)
      throw "makeRegPlus: Non plus seperator";
    tokens.pop_front();
    delete elt;
    if (tokens.size()==0)
      throw "makeRegPlus: Sum ends with plus";
    elt=tokens.front();
    if (elt->GetType()!=isExp)
      throw "makeRegPlus: Non expression element";
    tokens.pop_front();
    // combine re and new element by the sum constructor
    re=new RE_Sum(re,elt->ReleaseExp());
    delete elt;
  }
  return re;
} // }}}
// makeReg3 combines makeRegFlat, makeRegStar, makeRegSeq and makeRegPlus.
RE *makeReg3(list<ParseElt*> &tokens) // {{{
{ list<ParseElt*> tokens_flat;
  makeRegFlat(tokens,tokens_flat);
  list<ParseElt*> tokens_star;
  makeRegStar(tokens_flat,tokens_star);
  list<ParseElt*> tokens_seq;
  makeRegSeq(tokens_star,tokens_seq);
 return makeRegPlus(tokens_seq);
} // }}}

RE *RE::Create(const string &exp) // {{{
{
  try
  {
  list<ParseElt*> tokens1;
  makeReg1(exp,tokens1);
  list<ParseElt*> tokens2;
  makeReg2(tokens1,tokens2);
  RE *result=makeReg3(tokens2);
  return result;
  }
  catch (const char *err)
  { cerr << "Bad re: " << exp << endl
         << "Error was: " << err << endl;
    return new RE();
  }
} // }}}

// Implementation of Regular Expressions (RE)
RE::RE() // {{{
{
} // }}}
RE::~RE() // {{{
{
} // }}}
string RE::ToString(int level) const // {{{
{
  return "0";
} // }}}
RV *RE::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  return NULL; // Error: No value
} // }}}
RE *RE::Copy() const // {{{
{ return new RE();
} // }}}
bool RE::Accept(const std::string &s, int start, int end) const // {{{
{ return false;
} // }}}
BitCode RE::Compress(const std::string &s) const // {{{
{ bool accept;
  BitCode v=Compress(s,0,s.size(),accept);
  if (accept)
    return v;
  else
    throw (string)"Error: No Match";
} // }}}
BitCode RE::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ accept=false;
  return BitCode();
} // }}}

RE_One::RE_One() // {{{
{
} // }}}
RE_One::~RE_One() // {{{
{
} // }}}
string RE_One::ToString(int level) const // {{{
{
  return "1";
} // }}}
RV *RE_One::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  return new RV_Unit();
} // }}}
bool RE_One::Accept(const std::string &s, int start, int end) const // {{{
{ if (end<0)
    end=s.size();
  return start==end;
} // }}}
BitCode RE_One::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ if (end<0)
    end=s.size();
  if (start==end)
    accept=true;
  else
    accept=false;
  return BitCode();
} // }}}
RE_One *RE_One::Copy() const // {{{
{ return new RE_One();
} // }}}

RE_Char::RE_Char(char ch) // {{{
{
  myChar=ch;
} // }}}
RE_Char::~RE_Char() // {{{
{
} // }}}
string RE_Char::ToString(int level) const // {{{
{
  string s;
  switch (myChar)
  { case '(':
      s+="\\(";
      break;
    case ')':
      s+="\\)";
      break;
    case '+':
      s+="\\+";
      break;
    case '*':
      s+="\\*";
      break;
    default:
      s+=myChar;
  }
  return s;
} // }}}
RV *RE_Char::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  return new RV_Char(myChar);
} // }}}
bool RE_Char::Accept(const std::string &s, int start, int end) const // {{{
{ if (end<0)
    end=s.size();
  return start+1==end && s[start]==myChar;
} // }}}
BitCode RE_Char::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ if (end<0)
    end=s.size();
  if (start+1==end && s[start]==myChar)
    accept=true;
  else
    accept=false;
  return BitCode();
} // }}}
RE_Char *RE_Char::Copy() const // {{{
{ return new RE_Char(myChar);
} // }}}
char RE_Char::GetChar() const // {{{
{
  return myChar;
} // }}}

RE_Seq::RE_Seq(RE *left, RE *right) // {{{
{
  myLeft=left;
  myRight=right;
} // }}}
RE_Seq::~RE_Seq() // {{{
{
  delete myLeft;
  delete myRight;
} // }}}
string RE_Seq::ToString(int level) const // {{{
{
  if (level>2)
    return (string)"(" + myLeft->ToString(2) + myRight->ToString(2) + ")";
  else
    return myLeft->ToString(2) + myRight->ToString(2);
} // }}}
RV *RE_Seq::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  return new RV_Pair(myLeft->Decompress(bitvalue,pos),myRight->Decompress(bitvalue,pos));
} // }}}
bool RE_Seq::Accept(const std::string &s, int start, int end) const // {{{
{ if (end<0)
    end=s.size();
  for (int split=start; split<=end; ++split)
    if (myLeft->Accept(s,start,split) &&
        myRight->Accept(s,split,end))
      return true;
  return false;
} // }}}
BitCode RE_Seq::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ if (end<0)
    end=s.size();
  for (int split=end; split>=start; --split)
  { bool accept1;
    BitCode v1 = myLeft->Compress(s,start,split,accept1);
    bool accept2;
    BitCode v2 = myRight->Compress(s,split,end,accept2);
    if (accept1 && accept2)
    { accept=true;
      v1.Append(v2);
      return v1;
    }
  }
  accept=false;
  return BitCode();
} // }}}
RE_Seq *RE_Seq::Copy() const // {{{
{ return new RE_Seq(myLeft->Copy(),myRight->Copy());
} // }}}
const RE &RE_Seq::GetFront() const // {{{
{
  return *myLeft;
} // }}}
const RE &RE_Seq::GetBack() const // {{{
{
  return *myRight;
} // }}}

RE_Sum::RE_Sum(RE *left, RE *right) // {{{
{
  myLeft=left;
  myRight=right;
} // }}}
RE_Sum::~RE_Sum() // {{{
{
  delete myLeft;
  delete myRight;
} // }}}
string RE_Sum::ToString(int level) const // {{{
{
  if (level > 1)
    return (string)"(" + myLeft->ToString(1) + "+" + myRight->ToString(1) + ")";
  else
    return myLeft->ToString(1) + "+" + myRight->ToString(1);
} // }}}
RV *RE_Sum::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  if (pos>=bitvalue.GetLength())
    return NULL;
  if (not bitvalue.GetBit(pos++))
    return new RV_Inl(myLeft->Decompress(bitvalue,pos));
  else
    return new RV_Inr(myRight->Decompress(bitvalue,pos));
} // }}}
bool RE_Sum::Accept(const std::string &s, int start, int end) const // {{{
{ if (end<0)
    end=s.size();
  return myLeft->Accept(s,start,end) || myRight->Accept(s,start,end);
} // }}}
BitCode RE_Sum::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ if (end<0)
    end=s.size();
  BitCode v=myLeft->Compress(s,start,end,accept);
  if (accept)
  { BitCode result;
    result.PushBit(BitCode::INL);
    result.Append(v);
    return result;
  }
  v=myRight->Compress(s,start,end,accept);
  if (accept)
  { BitCode result;
    result.PushBit(BitCode::INR);
    result.Append(v);
    return result;
  }
  return BitCode();
} // }}}
RE_Sum *RE_Sum::Copy() const // {{{
{ return new RE_Sum(myLeft->Copy(),myRight->Copy());
} // }}}
const RE &RE_Sum::GetLeft() const // {{{
{
  return *myLeft;
} // }}}
const RE &RE_Sum::GetRight() const // {{{
{
  return *myRight;
} // }}}

RE_Star::RE_Star(RE *sub) // {{{
{
  mySub=sub;
} // }}}
RE_Star::~RE_Star() // {{{
{
  delete mySub;
} // }}}
string RE_Star::ToString(int level) const // {{{
{
  if (level > 3)
    return (string)"(" + mySub->ToString(3) + "*)";
  else
    return mySub->ToString(3) + "*";
} // }}}
RV *RE_Star::Decompress(const BitCode &bitvalue, int &pos) const // {{{
{
  if (pos>=bitvalue.GetLength())
    return NULL;
  if (not bitvalue.GetBit(pos++))
    return new RV_Fold(new RV_Inl(new RV_Unit));
  else
    return new RV_Fold(new RV_Inr(new RV_Pair(mySub->Decompress(bitvalue,pos),Decompress(bitvalue,pos))));
} // }}}
bool RE_Star::Accept(const std::string &s, int start, int end) const // {{{
{ if (end<0)
    end=s.size();
  if (start==end)
    return true;
  for (int split=end; split>start; --split) // Greedy approach
    if (mySub->Accept(s,start,split) && Accept(s,split,end))
      return true;
  return false;
} // }}}
BitCode RE_Star::Compress(const std::string &s, int start, int end, bool &accept) const // {{{
{ if (end<0)
    end=s.size();
  if (start==end)
  { accept= true;
    BitCode result;
    result.PushBit(BitCode::NIL);
    return result;
  }
  for (int split=end; split>start; --split) // Greedy approach
  { BitCode v1=mySub->Compress(s,start,split,accept);
    if (accept)
    { BitCode v2=Compress(s,split,end,accept);
      if (accept)
      { BitCode result;
        result.PushBit(BitCode::CONS);
	result.Append(v1);
	result.Append(v2);
        return result;
      }
    }
  }
  return BitCode();
} // }}}
RE_Star *RE_Star::Copy() const // {{{
{ return new RE_Star(mySub->Copy());
} // }}}
const RE &RE_Star::GetSub() const // {{{
{
  return *mySub;
} // }}}

// Implementation of RE Values (RV)
RV_Unit::RV_Unit() // {{{
{
} // }}}
RV_Unit::~RV_Unit() // {{{
{
} // }}}
string RV_Unit::Flatten() const // {{{
{
  return "";
} // }}}
string RV_Unit::ToString() const // {{{
{
  return "()";
} // }}}
BitCode RV_Unit::BitRep() const // {{{
{
  return BitCode();
} // }}}

RV_Char::RV_Char(char ch) // {{{
{
  myChar = ch;
} // }}}
RV_Char::~RV_Char() // {{{
{
} // }}}
string RV_Char::Flatten() const // {{{
{
  string result;
  result += myChar;
  return result;
} // }}}
string RV_Char::ToString() const // {{{
{
  string result;
  result += myChar;
  return result;
} // }}}
BitCode RV_Char::BitRep() const // {{{
{
  return BitCode();
} // }}}

RV_Inl::RV_Inl(RV *val) // {{{
{
  myVal = val;
} // }}}
RV_Inl::~RV_Inl() // {{{
{
  delete myVal;
} // }}}
string RV_Inl::Flatten() const // {{{
{
  return myVal->Flatten();
} // }}}
string RV_Inl::ToString() const // {{{
{
  return (string)"inl("+myVal->ToString()+")";
} // }}}
BitCode RV_Inl::BitRep() const // {{{
{
  BitCode result;
  result.PushBit(BitCode::INL);
  result.Append(myVal->BitRep());
  return BitCode();
} // }}}
const RV* RV_Inl::GetLeft() const // {{{
{ return myVal;
} // }}}

RV_Inr::RV_Inr(RV *val) // {{{
{
  myVal = val;
} // }}}
RV_Inr::~RV_Inr() // {{{
{
  delete myVal;
} // }}}
string RV_Inr::Flatten() const // {{{
{
  return myVal->Flatten();
} // }}}
string RV_Inr::ToString() const // {{{
{
  return (string)"inr("+myVal->ToString()+")";
} // }}}
BitCode RV_Inr::BitRep() const // {{{
{
  BitCode result;
  result.PushBit(BitCode::INR);
  result.Append(myVal->BitRep());
  return BitCode();
} // }}}
const RV* RV_Inr::GetRight() const // {{{
{ return myVal;
} // }}}

RV_Pair::RV_Pair(RV *front, RV *back) // {{{
{
  myFront = front;
  myBack = back;
} // }}}
RV_Pair::~RV_Pair() // {{{
{
  delete myFront;
  delete myBack;
} // }}}
string RV_Pair::Flatten() const // {{{
{
  return myFront->Flatten() + myBack->Flatten();
} // }}}
string RV_Pair::ToString() const // {{{
{
  return (string)"("+myFront->ToString()+"," + myBack->ToString() + ")";
} // }}}
BitCode RV_Pair::BitRep() const // {{{
{
  BitCode result = myFront->BitRep();
  result.Append(myBack->BitRep());
  return result;
} // }}}
const RV* RV_Pair::GetFront() const // {{{
{ return myFront;
} // }}}
const RV* RV_Pair::GetBack() const // {{{
{ return myBack;
} // }}}

RV_Fold::RV_Fold(RV *val) // {{{
{
  myVal = val;
} // }}}
RV_Fold::~RV_Fold() // {{{
{
  delete myVal;
} // }}}
string RV_Fold::Flatten() const // {{{
{
  return myVal->Flatten();
} // }}}
string RV_Fold::ToString() const // {{{
{
  return (string)"fold("+myVal->ToString()+")";
} // }}}
BitCode RV_Fold::BitRep() const // {{{
{
  return myVal->BitRep();
} // }}}
const RV* RV_Fold::GetSub() const // {{{
{ return myVal;
} // }}}

