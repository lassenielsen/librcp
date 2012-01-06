#include <rcp/frca.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

string PrintSuffixes(const set<int> suffixes) // {{{
{ stringstream out;
  out << "{";
  for (set<int>::const_iterator it=suffixes.begin(); it!=suffixes.end(); ++it)
  {
    if (it!=suffixes.begin())
      out << ", ";
    out << *it;
  }
  out << "}";
  return out.str();
} // }}}
FRCA *FRCA::Create(const RE *exp, int size) // {{{
{ if (typeid(*exp)==typeid(RE_One))
    return new FRCA_One();

  if (typeid(*exp)==typeid(RE_Char))
      return new FRCA_Char(((RE_Char*)exp)->GetChar());

  if (typeid(*exp)==typeid(RE_Seq))
      return new FRCA_Seq(FRCA::Create(&((const RE_Seq*)exp)->GetFront(), size),
                          FRCA::Create(&((const RE_Seq*)exp)->GetBack(), size));

  if (typeid(*exp)==typeid(RE_Sum))
      return new FRCA_Sum(FRCA::Create(&((const RE_Sum*)exp)->GetLeft(), size),
                          FRCA::Create(&((const RE_Sum*)exp)->GetRight(), size));

  if (typeid(*exp)==typeid(RE_Star))
      return new FRCA_Star(FRCA::Create(&((const RE_Star*)exp)->GetSub(), size));

  // Otherwise exp is an RE Object
  return new FRCA();
} // }}}

FRCA::FRCA() // {{{
{
} // }}}
FRCA::~FRCA() // {{{
{
} // }}}

bool FRCA::Accept() const // {{{
{ return HasSuffix(0);
} // }}}
BitCode FRCA::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  throw (string)"Error: No Match";
} // }}}
string FRCA::ToString() const // {{{
{ return PrintSuffixes(mySuffixes) + "0";
} // }}}
vector<int> FRCA::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  return result;
} // }}}
bool FRCA::HasSuffix(int pos) const // {{{
{ return mySuffixes.find(pos)!=mySuffixes.end();
} // }}}
int FRCA::MaxSuffix() const // {{{
{ if (mySuffixes.size()>0)
    return *mySuffixes.rbegin();
  return -1;
} // }}}

FRCA_One::FRCA_One() // {{{
{
} // }}}
FRCA_One::~FRCA_One() // {{{
{
} // }}}
BitCode FRCA_One::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
    return BitCode();
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_One::ToString() const // {{{
{ return PrintSuffixes(mySuffixes) + "1";
} // }}}
vector<int> FRCA_One::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  if (not HasSuffix(pos))
  { mySuffixes.insert(pos);
    result.push_back(pos);
  }
  return result;
} // }}}

FRCA_Char::FRCA_Char(char ch) // {{{
{
  myChar=ch;
} // }}}
FRCA_Char::~FRCA_Char() // {{{
{
} // }}}
BitCode FRCA_Char::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
  { ++pos;
    return BitCode();
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Char::ToString() const // {{{
{
  string s=PrintSuffixes(mySuffixes);
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
vector<int> FRCA_Char::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  if (pos>0 && s[pos-1]==myChar)
  { if (not HasSuffix(pos-1))
    { mySuffixes.insert(pos-1);
      result.push_back(pos-1);
    }
  }
  return result;
} // }}}
char FRCA_Char::GetChar() const // {{{
{ return myChar;
} // }}}

FRCA_Seq::FRCA_Seq(FRCA *left, FRCA *right) // {{{
{ myLeft=left;
  myRight=right;
} // }}}
FRCA_Seq::~FRCA_Seq() // {{{
{ delete myLeft;
  delete myRight;
} // }}}
BitCode FRCA_Seq::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
  { BitCode v1 = myLeft->Compress(pos);
    BitCode v2 = myRight->Compress(pos);
    v1.Append(v2);
    return v1;
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Seq::ToString() const // {{{
{ return PrintSuffixes(mySuffixes) + "(" + myLeft->ToString() + ")(" + myRight->ToString() + ")";
} // }}}
vector<int> FRCA_Seq::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  vector<int> result1 = myRight->AddSuffix(s,pos);
  for (vector<int>::const_iterator it1=result1.begin();it1!=result1.end();++it1)
  { vector<int> result2=myLeft->AddSuffix(s,*it1);
    for (vector<int>::const_iterator it2=result2.begin();it2!=result2.end();++it2)
    { if (not HasSuffix(*it2))
      { mySuffixes.insert(*it2);
        result.push_back(*it2);
      }
    }
  }
  return result;
} // }}}
const FRCA &FRCA_Seq::GetFront() const // {{{
{ return *myLeft;
} // }}}
const FRCA &FRCA_Seq::GetBack() const // {{{
{ return *myRight;
} // }}}

FRCA_Sum::FRCA_Sum(FRCA *left, FRCA *right) // {{{
{ myLeft=left;
  myRight=right;
} // }}}
FRCA_Sum::~FRCA_Sum() // {{{
{ delete myLeft;
  delete myRight;
} // }}}
BitCode FRCA_Sum::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (myLeft->HasSuffix(pos))
  { BitCode result;
    result.PushBit(BC_INL);
    result.Append(myLeft->Compress(pos));
    return result;
  }
  else if (myRight->HasSuffix(pos))
  { BitCode result;
    result.PushBit(BC_INR);
    result.Append(myRight->Compress(pos));
    return result;
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Sum::ToString() const // {{{
{ return PrintSuffixes(mySuffixes)
       + "(" + myLeft->ToString() + ")+(" + myRight->ToString() + ")";
} // }}}
vector<int> FRCA_Sum::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  vector<int> result1=myLeft->AddSuffix(s,pos);
  vector<int> result2=myRight->AddSuffix(s,pos);
  for (vector<int>::const_iterator it=result1.begin();it!=result1.end();++it)
  { if (not HasSuffix(*it))
    { mySuffixes.insert(*it);
      result.push_back(*it);
    }
  }
  for (vector<int>::const_iterator it=result2.begin();it!=result2.end();++it)
  { if (not HasSuffix(*it))
    { mySuffixes.insert(*it);
      result.push_back(*it);
    }
  }
  return result;
} // }}}
const FRCA &FRCA_Sum::GetLeft() const // {{{
{ return *myLeft;
} // }}}
const FRCA &FRCA_Sum::GetRight() const // {{{
{ return *myRight;
} // }}}

FRCA_Star::FRCA_Star(FRCA *sub) // {{{
{ mySub=sub;
} // }}}
FRCA_Star::~FRCA_Star() // {{{
{
  delete mySub;
} // }}}
BitCode FRCA_Star::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (not HasSuffix(pos))
    throw (string)"Error: No Match";
  BitCode result;
  int old_pos=pos;
  while (mySub->HasSuffix(pos))
  {
    result.PushBit(BC_CONS);
    result.Append(mySub->Compress(pos));
    if (pos == old_pos)
      throw (string)"Error: Infinite loop";
  }
  result.PushBit(BC_NIL);
  return result;
} // }}}
string FRCA_Star::ToString() const // {{{
{ return PrintSuffixes(mySuffixes) + "(" + mySub->ToString() + ")*";
} // }}}
vector<int> FRCA_Star::AddSuffix(const string &s, int pos) // {{{
{
  vector<int> result;
  vector<int> workset;
  workset.push_back(pos);
  while (workset.size()>0)
  {
    int cur_pos = workset.back();
    workset.pop_back();
    if (not HasSuffix(cur_pos))
    { mySuffixes.insert(cur_pos);
      result.push_back(cur_pos);
      vector<int> result1=mySub->AddSuffix(s,cur_pos);
      workset.insert(workset.end(),result1.begin(),result1.end());
    }
  }
  return result;
} // }}}
const FRCA &FRCA_Star::GetSub() const // {{{
{ return *mySub;
} // }}}
