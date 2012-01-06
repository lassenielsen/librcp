#include <rcp/bitcode.hpp>
#include <rcp/bitcode_order.hpp>
using namespace std;

bool BitCode::INL=false;
char BitCode::INL_CHR='0';
string BitCode::INL_STR="0";
bool BitCode::INR=true;
char BitCode::INR_CHR='1';
string BitCode::INR_STR="1";
bool BitCode::CONS=false;
char BitCode::CONS_CHR='0';
string BitCode::CONS_STR="0";
bool BitCode::NIL=true;
char BitCode::NIL_CHR='1';
string BitCode::NIL_STR="1";
BitCode::BitCode() // {{{
: myLength(0)
{
} // }}}
BitCode::~BitCode() // {{{
{
} // }}}
unsigned int BitCode::GetLength() const // {{{
{ return myLength;
} // }}}
bool BitCode::GetBit(unsigned int index) const // {{{
{ if (index>=myLength)
    throw (string)"BitCode::GetBit index out of bounds";
  return (myBits[index/8])&(1<<(index%8));
} // }}}
void BitCode::PushBit(bool bit) // {{{
{ if (myLength%8==0)
    myBits.push_back('\0');
  if (bit)
    myBits.back()=myBits.back()|(1<<(myLength%8));
  ++myLength;
  return;
} // }}}
void BitCode::Append(const BitCode &source) // {{{
{ for (int i=0;i<source.GetLength(); ++i)
    PushBit(source.GetBit(i));
  return;
} // }}}
void BitCode::Append(const string &source) // {{{
{ for (int i=0;i<source.size(); ++i)
    PushBit(source[i]=='1');
  return;
} // }}}
BitCode BitCode::Reverse() // {{{
{ BitCode result;
  for (int i=myLength-1;i>=0; --i)
    result.PushBit(GetBit(i));
  return result;
} // }}}
string BitCode::ToString() const // {{{
{ string result;
  for (unsigned int i=0; i<myLength; ++i)
    if (GetBit(i))
      result += "1";
    else
      result += "0";

  return result;
} // }}}
