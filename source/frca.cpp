#include <rcp/frca.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

string PrintSuffixes(const set<int> suffixes, const set<int> productive_suffixes) // {{{
{ stringstream out;
  out << "{";
  for (set<int>::const_iterator it=suffixes.begin(); it!=suffixes.end(); ++it)
  {
    if (it!=suffixes.begin())
      out << ", ";
    out << *it;
  }
  out << "/";
  for (set<int>::const_iterator it=productive_suffixes.begin(); it!=productive_suffixes.end(); ++it)
  {
    if (it!=productive_suffixes.begin())
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
{ return HasSuffix(0,false);
} // }}}
BitCode FRCA::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  throw (string)"Error: No Match";
} // }}}
string FRCA::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,myProductiveSuffixes) + "0";
} // }}}
vector<worklistitem> FRCA::AddSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  return result;
} // }}}
bool FRCA::HasSuffix(int pos, bool onlyProductive) const // {{{
{ if (myProductiveSuffixes.find(pos)!=myProductiveSuffixes.end())
    return true;
  else if (onlyProductive)
    return false;
  else
    return mySuffixes.find(pos)!=mySuffixes.end();
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
BitCode FRCA_One::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos,productive))
    return BitCode();
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_One::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,myProductiveSuffixes) + "1";
} // }}}
vector<worklistitem> FRCA_One::AddSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (not HasSuffix(pos,productive))
  { if (productive)
      myProductiveSuffixes.insert(pos);
    else
      mySuffixes.insert(pos);
    worklistitem item;
    item.pos=pos;
    item.productive=productive;
    result.push_back(item);
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
BitCode FRCA_Char::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos,productive))
  { ++pos;
    return BitCode();
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Char::ToString() const // {{{
{
  string s=PrintSuffixes(mySuffixes,myProductiveSuffixes);
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
vector<worklistitem> FRCA_Char::AddSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (pos>0 && s[pos-1]==myChar)
  { if (not HasSuffix(pos-1,productive))
    { myProductiveSuffixes.insert(pos-1);
      worklistitem item;
      item.pos=pos-1;
      item.productive=true;
      result.push_back(item);
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
BitCode FRCA_Seq::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos,productive))
  { int origpos=pos;
    BitCode v1 = myLeft->Compress(pos,productive);
    bool productive2=origpos==pos?productive:false;
    BitCode v2 = myRight->Compress(pos,productive2);
    v1.Append(v2);
    return v1;
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Seq::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,myProductiveSuffixes) + "(" + myLeft->ToString() + ")(" + myRight->ToString() + ")";
} // }}}
vector<worklistitem> FRCA_Seq::AddSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1 = myRight->AddSuffix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it1=result1.begin();it1!=result1.end();++it1)
  { vector<worklistitem> result2=myLeft->AddSuffix(s,it1->pos,it1->productive);
    for (vector<worklistitem>::const_iterator it2=result2.begin();it2!=result2.end();++it2)
    { if (not HasSuffix(it2->pos,it2->productive))
      { if (it2->productive)
          myProductiveSuffixes.insert(it2->pos);
        else
          mySuffixes.insert(it2->pos);
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
BitCode FRCA_Sum::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (myLeft->HasSuffix(pos,productive))
  { BitCode result;
    result.PushBit(BitCode::INL);
    result.Append(myLeft->Compress(pos,productive));
    return result;
  }
  else if (myRight->HasSuffix(pos,productive))
  { BitCode result;
    result.PushBit(BitCode::INR);
    result.Append(myRight->Compress(pos,productive));
    return result;
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Sum::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,myProductiveSuffixes)
       + "(" + myLeft->ToString() + ")+(" + myRight->ToString() + ")";
} // }}}
vector<worklistitem> FRCA_Sum::AddSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1=myLeft->AddSuffix(s,pos,productive);
  vector<worklistitem> result2=myRight->AddSuffix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it=result1.begin();it!=result1.end();++it)
  { if (not HasSuffix(it->pos,it->productive))
    { if (it->productive)
        myProductiveSuffixes.insert(it->pos);
      else
        mySuffixes.insert(it->pos);
      result.push_back(*it);
    }
  }
  for (vector<worklistitem>::const_iterator it=result2.begin();it!=result2.end();++it)
  { if (not HasSuffix(it->pos,it->productive))
    { if (it->productive)
        myProductiveSuffixes.insert(it->pos);
      else
        mySuffixes.insert(it->pos);
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
BitCode FRCA_Star::Compress(int &pos, bool productive) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (not HasSuffix(pos,productive))
    throw (string)"Error: No Match";
  BitCode result;
  int old_pos=pos;
  while (mySub->HasSuffix(pos,true))
  {
    result.PushBit(BitCode::CONS);
    result.Append(mySub->Compress(pos,true));
    if (pos == old_pos)
      throw (string)"Error: Infinite loop";
  }
  result.PushBit(BitCode::NIL);
  return result;
} // }}}
string FRCA_Star::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,myProductiveSuffixes) + "(" + mySub->ToString() + ")*";
} // }}}
vector<worklistitem> FRCA_Star::AddSuffix(const string &s, int pos, bool productive) // {{{
{
  vector<worklistitem> result;
  vector<worklistitem> workset;
  worklistitem startitem;
  startitem.pos=pos;
  startitem.productive=productive;
  workset.push_back(startitem);
  while (workset.size()>0)
  {
    worklistitem item = workset.back();
    workset.pop_back();
    if (not HasSuffix(item.pos,item.productive))
    { if (item.productive)
        myProductiveSuffixes.insert(item.pos);
      else
        mySuffixes.insert(item.pos);
      result.push_back(item);
      vector<worklistitem> subResult=mySub->AddSuffix(s,item.pos,false);
      workset.insert(workset.end(),subResult.begin(),subResult.end());
    }
  }
  return result;
} // }}}
const FRCA &FRCA_Star::GetSub() const // {{{
{ return *mySub;
} // }}}
