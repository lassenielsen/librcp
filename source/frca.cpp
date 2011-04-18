#include <rcp/frca.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

string PrintSuffixes(const bool *suffixes, int size) // {{{
{ stringstream out;
  out << "{";
  for (int i=0; i<size; ++i)
    if (suffixes[i])
      out << "," << i;
  
  out << "}";
  return out.str();
} // }}}
FRCA *FRCA::Create(const RE *exp, int size) // {{{
{ if (typeid(*exp)==typeid(RE_One))
    return new FRCA_One(size);

  if (typeid(*exp)==typeid(RE_Char))
      return new FRCA_Char(size, ((RE_Char*)exp)->GetChar());

  if (typeid(*exp)==typeid(RE_Seq))
      return new FRCA_Seq(size,
                          FRCA::Create(&((const RE_Seq*)exp)->GetFront(), size),
                          FRCA::Create(&((const RE_Seq*)exp)->GetBack(), size));

  if (typeid(*exp)==typeid(RE_Sum))
      return new FRCA_Sum(size,
                          FRCA::Create(&((const RE_Sum*)exp)->GetLeft(), size),
                          FRCA::Create(&((const RE_Sum*)exp)->GetRight(), size));

  if (typeid(*exp)==typeid(RE_Star))
      return new FRCA_Star(size,
                           FRCA::Create(&((const RE_Star*)exp)->GetSub(), size));

  // Otherwise exp is an RE Object
  return new FRCA(size);
} // }}}

FRCA::FRCA(int size) // {{{
{
  mySize=size+1;
  mySuffixes = new bool[mySize];
  for (int i=0; i<mySize; ++i)
    mySuffixes[i]=false;
} // }}}
FRCA::~FRCA() // {{{
{
  delete [] mySuffixes;
} // }}}

bool FRCA::Accept() const // {{{
{ return HasSuffix(0);
} // }}}
string FRCA::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  return "Error: Not member";
} // }}}
string FRCA::ToString() const // {{{
{ return PrintSuffixes(mySuffixes, mySize) + "0";
} // }}}
vector<int> FRCA::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  return result;
} // }}}
bool FRCA::HasSuffix(int pos) const // {{{
{  if (pos<mySize)
     return mySuffixes[pos];
   return false;
} // }}}
int FRCA::MaxSuffix() const // {{{
{  for (int i=mySize-1; i>=0; --i)
     if (mySuffixes[i])
       return i;
   return -1;
} // }}}

FRCA_One::FRCA_One(int size) // {{{
: FRCA(size)
{
} // }}}
FRCA_One::~FRCA_One() // {{{
{
} // }}}
string FRCA_One::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
    return "";
  else
    return "Error: Not member";
} // }}}
string FRCA_One::ToString() const // {{{
{ return PrintSuffixes(mySuffixes,mySize) + "1";
} // }}}
vector<int> FRCA_One::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  if (not HasSuffix(pos))
  { mySuffixes[pos]=true;
    result.push_back(pos);
  }
  return result;
} // }}}

FRCA_Char::FRCA_Char(int size, char ch) // {{{
: FRCA(size)
{
  myChar=ch;
} // }}}
FRCA_Char::~FRCA_Char() // {{{
{
} // }}}
string FRCA_Char::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
  { ++pos;
    return "";
  }
  else
    return "Error: Not member";
} // }}}
string FRCA_Char::ToString() const // {{{
{
  string s=PrintSuffixes(mySuffixes, mySize);
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
  { if (not mySuffixes[pos-1])
    { mySuffixes[pos-1]=true;
      result.push_back(pos-1);
    }
  }
  return result;
} // }}}
char FRCA_Char::GetChar() const // {{{
{ return myChar;
} // }}}

FRCA_Seq::FRCA_Seq(int size, FRCA *left, FRCA *right) // {{{
: FRCA(size)
{ myLeft=left;
  myRight=right;
} // }}}
FRCA_Seq::~FRCA_Seq() // {{{
{ delete myLeft;
  delete myRight;
} // }}}
string FRCA_Seq::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (HasSuffix(pos))
  { string v1 = myLeft->Compress(pos);
    string v2 = myRight->Compress(pos);
    return v1 + v2;
  }
  else
    return "Error: Not member";
} // }}}
string FRCA_Seq::ToString() const // {{{
{ return PrintSuffixes(mySuffixes, mySize) + "(" + myLeft->ToString() + ")(" + myRight->ToString() + ")";
} // }}}
vector<int> FRCA_Seq::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  vector<int> result1 = myRight->AddSuffix(s,pos);
  for (vector<int>::const_iterator it1=result1.begin();it1!=result1.end();++it1)
  { vector<int> result2=myLeft->AddSuffix(s,*it1);
    for (vector<int>::const_iterator it2=result2.begin();it2!=result2.end();++it2)
    { if (not mySuffixes[*it2])
      { mySuffixes[*it2]=true;
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

FRCA_Sum::FRCA_Sum(int size, FRCA *left, FRCA *right) // {{{
: FRCA(size)
{ myLeft=left;
  myRight=right;
} // }}}
FRCA_Sum::~FRCA_Sum() // {{{
{ delete myLeft;
  delete myRight;
} // }}}
string FRCA_Sum::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (myLeft->HasSuffix(pos))
    return myLeft->Compress(pos).insert(0,"0");
  else if (myRight->HasSuffix(pos))
    return myRight->Compress(pos).insert(0,"1");
  else
    return "Error: Not member";
} // }}}
string FRCA_Sum::ToString() const // {{{
{ return PrintSuffixes(mySuffixes, mySize)
       + "(" + myLeft->ToString() + ")+(" + myRight->ToString() + ")";
} // }}}
vector<int> FRCA_Sum::AddSuffix(const string &s, int pos) // {{{
{ vector<int> result;
  vector<int> result1=myLeft->AddSuffix(s,pos);
  vector<int> result2=myRight->AddSuffix(s,pos);
  for (vector<int>::const_iterator it=result1.begin();it!=result1.end();++it)
  { if (not mySuffixes[*it])
    { mySuffixes[*it]=true;
      result.push_back(*it);
    }
  }
  for (vector<int>::const_iterator it=result2.begin();it!=result2.end();++it)
  { if (not mySuffixes[*it])
    { mySuffixes[*it]=true;
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

FRCA_Star::FRCA_Star(int size, FRCA *sub) // {{{
: FRCA(size)
{ mySub=sub;
} // }}}
FRCA_Star::~FRCA_Star() // {{{
{
  delete mySub;
} // }}}
string FRCA_Star::Compress(int &pos) const // {{{
{ //cout << "Pos: " << pos << "FRCA: " << ToString() << endl;
  if (not HasSuffix(pos))
    return "Error: Not Member";
  string result="";
  int old_pos=pos;
  while (mySub->HasSuffix(pos))
  {
    result+=(mySub->Compress(pos).insert(0,"1"));
    if (pos == old_pos)
      return "Error: Infinite loop";
  }
  result += "0";
  return result;
} // }}}
string FRCA_Star::ToString() const // {{{
{ return PrintSuffixes(mySuffixes, mySize) + "(" + mySub->ToString() + ")*";
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
    if (not mySuffixes[cur_pos])
    { mySuffixes[cur_pos]=true;
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
