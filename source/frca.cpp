#include <rcp/frca.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <algorithm>

using namespace std;

string PrintMarks(const set<int> &marks1, const set<int> &marks2, const set<int> &marks3, const set<int> &marks4) // {{{
{ stringstream out;
  out << "{";
  for (set<int>::const_iterator it=marks1.begin(); it!=marks1.end(); ++it)
  {
    if (it!=marks1.begin())
      out << ", ";
    out << *it;
  }
  out << ";";
  for (set<int>::const_iterator it=marks2.begin(); it!=marks2.end(); ++it)
  {
    if (it!=marks2.begin())
      out << ", ";
    out << *it;
  }
  out << ";";
  for (set<int>::const_iterator it=marks3.begin(); it!=marks3.end(); ++it)
  {
    if (it!=marks3.begin())
      out << ", ";
    out << *it;
  }
  out << ";";
  for (set<int>::const_iterator it=marks4.begin(); it!=marks4.end(); ++it)
  {
    if (it!=marks4.begin())
      out << ", ";
    out << *it;
  }
  out << "}";
  return out.str();
} // }}}
FRCA *FRCA::Create(const RE *exp) // {{{
{ if (typeid(*exp)==typeid(RE_One))
    return new FRCA_One();

  if (typeid(*exp)==typeid(RE_Char))
      return new FRCA_Char(((RE_Char*)exp)->GetChar());

  if (typeid(*exp)==typeid(RE_Seq))
      return new FRCA_Seq(FRCA::Create(&((const RE_Seq*)exp)->GetFront()),
                          FRCA::Create(&((const RE_Seq*)exp)->GetBack()));

  if (typeid(*exp)==typeid(RE_Sum))
      return new FRCA_Sum(FRCA::Create(&((const RE_Sum*)exp)->GetLeft()),
                          FRCA::Create(&((const RE_Sum*)exp)->GetRight()));

  if (typeid(*exp)==typeid(RE_Star))
      return new FRCA_Star(FRCA::Create(&((const RE_Star*)exp)->GetSub()));

  // Otherwise exp is an RE Object
  return new FRCA();
} // }}}

FRCA::FRCA() // {{{
{
} // }}}
FRCA::~FRCA() // {{{
{
} // }}}

bool FRCA::Accept(const string &s) // {{{
{ ClearSuffixes();
  MarkSuffix(s,s.size(),true);
  return HasSuffix(0,false);
} // }}}
BitCode FRCA::CompressGL(const string &s) // {{{
{ ClearPrefixes();
  ClearSuffixes();
  MarkSuffix(s,s.size(),true);
  int pos=0;
  return CompressGL(pos,false);
} // }}}
BitCode FRCA::CompressLL(const string &s) // {{{
{ ClearPrefixes();
  ClearSuffixes();
  BitCode result;
  CompressLL(s,0,s.size(),result);
  return result;
} // }}}
BitCode FRCA::CompressGL(int &pos, bool productive) const // {{{
{
  throw (string)"Error: No Match";
} // }}}
void FRCA::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{
  throw (string)"Error: No Match";
} // }}}
string FRCA::ToString() const // {{{
{ return PrintMarks(myPrefixes,myProductivePrefixes,
                    mySuffixes,myProductiveSuffixes) + "0";
} // }}}
vector<worklistitem> FRCA::MarkSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  return result;
} // }}}
vector<worklistitem> FRCA::MarkPrefix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  return result;
} // }}}
void FRCA::ClearSuffixes() // {{{
{ mySuffixes.clear();
  myProductiveSuffixes.clear();
} // }}}
void FRCA::ClearPrefixes() // {{{
{ myPrefixes.clear();
  myProductivePrefixes.clear();
} // }}}
bool FRCA::HasSuffix(int pos, bool onlyProductive) const // {{{
{ if (myProductiveSuffixes.find(pos)!=myProductiveSuffixes.end())
    return true;
  else if (onlyProductive)
    return false;
  else
    return mySuffixes.find(pos)!=mySuffixes.end();
} // }}}
bool FRCA::HasPrefix(int pos, bool onlyProductive) const // {{{
{ if (myProductivePrefixes.find(pos)!=myProductivePrefixes.end())
    return true;
  else if (onlyProductive)
    return false;
  else
    return myPrefixes.find(pos)!=myPrefixes.end();
} // }}}
bool FRCA::SetSuffix(int pos, bool productive) // {{{
{ if (HasSuffix(pos,productive))
    return true;
  if (productive)
  { myProductiveSuffixes.insert(pos);
    mySuffixes.erase(pos); // Position implied by productive mark
  }
  else 
    mySuffixes.insert(pos);
  return false;
} // }}}
bool FRCA::SetPrefix(int pos, bool productive) // {{{
{ if (HasPrefix(pos,productive))
    return true;
  if (productive)
  { myProductivePrefixes.insert(pos);
    myPrefixes.erase(pos); // Position implied by productive mark
  }
  else 
    myPrefixes.insert(pos);
  return false;
} // }}}
int FRCA::MaxSuffix() const // {{{
{ if (mySuffixes.size()>0)
    return *mySuffixes.rbegin();
  return -1;
} // }}}
int FRCA::MaxPrefix() const // {{{
{ if (myPrefixes.size()>0)
    return *myPrefixes.rbegin();
  return -1;
} // }}}
set<int> FRCA::GetSuffixes() const // {{{
{ set<int> result;
  result.insert(mySuffixes.begin(),mySuffixes.end());
  result.insert(myProductiveSuffixes.begin(),myProductiveSuffixes.end());
  return result;
} // }}}
set<int> FRCA::GetPrefixes() const // {{{
{ set<int> result;
  result.insert(myPrefixes.begin(),myPrefixes.end());
  result.insert(myProductivePrefixes.begin(),myProductivePrefixes.end());
  return result;
} // }}}
FRCA *FRCA::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA *result = new FRCA();
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
  }
  return result;
} // }}}

FRCA_One::FRCA_One() // {{{
{
} // }}}
FRCA_One::~FRCA_One() // {{{
{
} // }}}
BitCode FRCA_One::CompressGL(int &pos, bool productive) const // {{{
{
  if (HasSuffix(pos,productive))
    return BitCode();
  else
    throw (string)"Error: No Match";
} // }}}
void FRCA_One::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{ 
  if (pos1!=pos2)
    throw (string)"Error: No Match";
} // }}}
string FRCA_One::ToString() const // {{{
{ return PrintMarks(myPrefixes,myProductivePrefixes,mySuffixes,myProductiveSuffixes) + "1";
} // }}}
vector<worklistitem> FRCA_One::MarkSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (not HasSuffix(pos,productive))
  { SetSuffix(pos, productive);
    worklistitem item;
    item.pos=pos;
    item.productive=productive;
    result.push_back(item);
  }
  return result;
} // }}}
vector<worklistitem> FRCA_One::MarkPrefix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (not HasPrefix(pos,productive))
  { SetPrefix(pos, productive);
    worklistitem item;
    item.pos=pos;
    item.productive=productive;
    result.push_back(item);
  }
  return result;
} // }}}
FRCA_One *FRCA_One::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA_One *result=new FRCA_One();
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
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
BitCode FRCA_Char::CompressGL(int &pos, bool productive) const // {{{
{
  if (HasSuffix(pos,productive))
  { ++pos;
    return BitCode();
  }
  else
    throw (string)"Error: No Match";
} // }}}
void FRCA_Char::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{
  if (pos2!=pos1+1 || s[pos1]!=myChar)
    throw (string)"Error: No Match";
} // }}}
string FRCA_Char::ToString() const // {{{
{
  string s=PrintMarks(myPrefixes,myProductivePrefixes,mySuffixes,myProductiveSuffixes);
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
vector<worklistitem> FRCA_Char::MarkSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (pos>0 && s[pos-1]==myChar)
  { if (not HasSuffix(pos-1,true))
    { SetSuffix(pos-1,true);
      worklistitem item;
      item.pos=pos-1;
      item.productive=true;
      result.push_back(item);
    }
  }
  return result;
} // }}}
vector<worklistitem> FRCA_Char::MarkPrefix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  if (pos<s.size() && s[pos]==myChar)
  { if (not HasPrefix(pos+1,true))
    { SetPrefix(pos+1,productive);
      worklistitem item;
      item.pos=pos+1;
      item.productive=true;
      result.push_back(item);
    }
  }
  return result;
} // }}}
FRCA_Char *FRCA_Char::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA_Char *result=new FRCA_Char(GetChar());
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
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
BitCode FRCA_Seq::CompressGL(int &pos, bool productive) const // {{{
{
  if (HasSuffix(pos,productive))
  { int origpos=pos;
    BitCode v1 = myLeft->CompressGL(pos,productive);
    bool productive2=origpos==pos?productive:false;
    BitCode v2 = myRight->CompressGL(pos,productive2);
    v1.Append(v2);
    return v1;
  }
  else
    throw (string)"Error: No Match";
} // }}}
// Function: max_common {{{
// Method to find the largest common element in two sets of non-negative integers
// Returns -1 if no common elements
int max_common(const set<int> &set1, const set<int> &set2)
{ int result=-1;
  set<int>::const_iterator it1=set1.begin();
  set<int>::const_iterator it2=set2.begin();
  while (it1!=set1.end() &&
         it2!=set2.end())
  { if (*it1==*it2)
    { result=*it1;
      ++it1;
      ++it2;
    }
    else if (*it1 < *it2)
      ++it1;
    else ++it2;
  }
  return result;
} // }}}
void FRCA_Seq::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{
  myLeft->MarkPrefix(s,pos1,true);
  myRight->MarkSuffix(s,pos2,true);
  set<int> splitLeft=myLeft->GetPrefixes();
  set<int> splitRight=myRight->GetSuffixes();
  int split=max_common(splitLeft,splitRight);
  if (split<0)
    throw (string)"Error: No Match";
  myLeft->CompressLL(s,pos1,split,dest);
  myRight->CompressLL(s,split,pos2,dest);
} // }}}
string FRCA_Seq::ToString() const // {{{
{ return PrintMarks(myPrefixes,myProductivePrefixes,mySuffixes,myProductiveSuffixes) + "(" + myLeft->ToString() + ")(" + myRight->ToString() + ")";
} // }}}
vector<worklistitem> FRCA_Seq::MarkSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1 = myRight->MarkSuffix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it1=result1.begin();it1!=result1.end();++it1)
  { vector<worklistitem> result2=myLeft->MarkSuffix(s,it1->pos,it1->productive);
    for (vector<worklistitem>::const_iterator it2=result2.begin();it2!=result2.end();++it2)
    { if (not HasSuffix(it2->pos,it2->productive))
      { SetSuffix(it2->pos, it2->productive);
        result.push_back(*it2);
      }
    }
  }
  return result;
} // }}}
vector<worklistitem> FRCA_Seq::MarkPrefix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1 = myLeft->MarkPrefix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it1=result1.begin();it1!=result1.end();++it1)
  { vector<worklistitem> result2=myRight->MarkPrefix(s,it1->pos,it1->productive);
    for (vector<worklistitem>::const_iterator it2=result2.begin();it2!=result2.end();++it2)
    {  if (not HasPrefix(it2->pos,it2->productive))
       { SetPrefix(it2->pos, it2->productive);
         result.push_back(*it2);
       }
    }
  }
  return result;
} // }}}
void FRCA_Seq::ClearSuffixes() // {{{
{ mySuffixes.clear();
  myProductiveSuffixes.clear();
  myLeft->ClearSuffixes();
  myRight->ClearSuffixes();
} // }}}
void FRCA_Seq::ClearPrefixes() // {{{
{ myPrefixes.clear();
  myProductivePrefixes.clear();
  myLeft->ClearPrefixes();
  myRight->ClearPrefixes();
} // }}}
FRCA_Seq *FRCA_Seq::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA_Seq *result=new FRCA_Seq(myLeft->Copy(), myRight->Copy());
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
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
BitCode FRCA_Sum::CompressGL(int &pos, bool productive) const // {{{
{
  if (myLeft->HasSuffix(pos,productive))
  { BitCode result;
    result.PushBit(BitCode::INL);
    result.Append(myLeft->CompressGL(pos,productive));
    return result;
  }
  else if (myRight->HasSuffix(pos,productive))
  { BitCode result;
    result.PushBit(BitCode::INR);
    result.Append(myRight->CompressGL(pos,productive));
    return result;
  }
  else
    throw (string)"Error: No Match";
} // }}}
void FRCA_Sum::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{
  myLeft->MarkSuffix(s,pos2,true);
  myRight->MarkSuffix(s,pos2,true);
  if (myLeft->HasSuffix(pos1,false))
  { dest.PushBit(BitCode::INL);
    myLeft->CompressLL(s,pos1,pos2,dest);
    return;
  }
  else if (myRight->HasSuffix(pos1,false))
  { dest.PushBit(BitCode::INR);
    myRight->CompressLL(s,pos1,pos2,dest);
    return;
  }
  else
    throw (string)"Error: No Match";
} // }}}
string FRCA_Sum::ToString() const // {{{
{ return PrintMarks(myPrefixes,myProductivePrefixes,mySuffixes,myProductiveSuffixes)
       + "(" + myLeft->ToString() + ")+(" + myRight->ToString() + ")";
} // }}}
vector<worklistitem> FRCA_Sum::MarkSuffix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1=myLeft->MarkSuffix(s,pos,productive);
  vector<worklistitem> result2=myRight->MarkSuffix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it=result1.begin();it!=result1.end();++it)
  { if (not HasSuffix(it->pos,it->productive))
    { SetSuffix(it->pos,it->productive);
      result.push_back(*it);
    }
  }
  for (vector<worklistitem>::const_iterator it=result2.begin();it!=result2.end();++it)
  { if (not HasSuffix(it->pos,it->productive))
    { SetSuffix(it->pos,it->productive);
      result.push_back(*it);
    }
  }
  return result;
} // }}}
vector<worklistitem> FRCA_Sum::MarkPrefix(const string &s, int pos, bool productive) // {{{
{ vector<worklistitem> result;
  vector<worklistitem> result1=myLeft->MarkPrefix(s,pos,productive);
  vector<worklistitem> result2=myRight->MarkPrefix(s,pos,productive);
  for (vector<worklistitem>::const_iterator it=result1.begin();it!=result1.end();++it)
  { if (not HasPrefix(it->pos,it->productive))
    { SetPrefix(it->pos,it->productive);
      result.push_back(*it);
    }
  }
  for (vector<worklistitem>::const_iterator it=result2.begin();it!=result2.end();++it)
  { if (not HasPrefix(it->pos,it->productive))
    { SetPrefix(it->pos,it->productive);
      result.push_back(*it);
    }
  }
  return result;
} // }}}
void FRCA_Sum::ClearSuffixes() // {{{
{ mySuffixes.clear();
  myProductiveSuffixes.clear();
  myLeft->ClearSuffixes();
  myRight->ClearSuffixes();
} // }}}
void FRCA_Sum::ClearPrefixes() // {{{
{ myPrefixes.clear();
  myProductivePrefixes.clear();
  myLeft->ClearPrefixes();
  myRight->ClearPrefixes();
} // }}}
FRCA_Sum *FRCA_Sum::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA_Sum *result=new FRCA_Sum(myLeft->Copy(), myRight->Copy());
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
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
BitCode FRCA_Star::CompressGL(int &pos, bool productive) const // {{{
{
  if (not HasSuffix(pos,productive))
    throw (string)"Error: No Match";
  BitCode result;
  int old_pos=pos;
  while (mySub->HasSuffix(pos,true))
  {
    result.PushBit(BitCode::CONS);
    result.Append(mySub->CompressGL(pos,true));
    if (pos == old_pos)
      throw (string)"Error: Infinite loop";
  }
  result.PushBit(BitCode::NIL);
  return result;
} // }}}
void FRCA_Star::CompressLL(const string &s, int pos1, int pos2, BitCode &dest) // {{{
{
  if (pos1==pos2)
  { dest.PushBit(BitCode::NIL);
    return;
  }
  // Find split between a single iteration (from left) and any number of iterations (from right)
  MarkSuffix(s,pos2,true);
  ClearPrefixes();
  mySub->MarkPrefix(s,pos1,true);
  int split=max_common(mySub->GetPrefixes(),GetSuffixes());
  if (split<0)
    throw (string)"Error: No Match";
  dest.PushBit(BitCode::CONS);
  FRCA *cpySub = mySub->Copy(true,false);
  cpySub->CompressLL(s,pos1,split,dest);
  delete cpySub;
  // Clear prefixes compress remaining with the found suffixes
  ClearPrefixes();
  CompressLL(s,split,pos2,dest);
} // }}}
string FRCA_Star::ToString() const // {{{
{ return PrintMarks(myPrefixes,myProductivePrefixes,mySuffixes,myProductiveSuffixes) + "(" + mySub->ToString() + ")*";
} // }}}
vector<worklistitem> FRCA_Star::MarkSuffix(const string &s, int pos, bool productive) // {{{
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
    { SetSuffix(item.pos,item.productive);
      result.push_back(item);
      vector<worklistitem> subResult=mySub->MarkSuffix(s,item.pos,false);
      workset.insert(workset.end(),subResult.begin(),subResult.end());
    }
  }
  return result;
} // }}}
vector<worklistitem> FRCA_Star::MarkPrefix(const string &s, int pos, bool productive) // {{{
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
    if (not HasPrefix(item.pos,item.productive))
    { SetPrefix(item.pos,item.productive);
      result.push_back(item);
      vector<worklistitem> subResult=mySub->MarkPrefix(s,item.pos,false);
      workset.insert(workset.end(),subResult.begin(),subResult.end());
    }
  }
  return result;
} // }}}
void FRCA_Star::ClearSuffixes() // {{{
{ mySuffixes.clear();
  myProductiveSuffixes.clear();
  mySub->ClearSuffixes();
} // }}}
void FRCA_Star::ClearPrefixes() // {{{
{ myPrefixes.clear();
  myProductivePrefixes.clear();
  mySub->ClearPrefixes();
} // }}}
FRCA_Star *FRCA_Star::Copy(bool copyPrefixes, bool copySuffixes) const // {{{
{
  FRCA_Star *result=new FRCA_Star(mySub->Copy());
  if (copyPrefixes)
  { result->myPrefixes = myPrefixes;
    result->myProductivePrefixes = myProductivePrefixes;
  }
  if (copySuffixes)
  { result->mySuffixes = mySuffixes;
    result->myProductiveSuffixes = myProductiveSuffixes;
  }
  return result;
} // }}}
const FRCA &FRCA_Star::GetSub() const // {{{
{ return *mySub;
} // }}}
