#include <rcp/bitcode.hpp>
#include <rcp/bitcode_order.hpp>
using namespace std;

const char TRUE_CHR = '1';
const char FALSE_CHR = '0';
const string TRUE_STR = "1";
const string FALSE_STR = "0";
const bool BitCode::INL=false;
const char BitCode::INL_CHR=BitCode::INL?TRUE_CHR:FALSE_CHR;
const string BitCode::INL_STR=BitCode::INL?TRUE_STR:FALSE_STR;
const bool BitCode::INR=true;
const char BitCode::INR_CHR=BitCode::INR?TRUE_CHR:FALSE_CHR;
const string BitCode::INR_STR=BitCode::INR?TRUE_STR:FALSE_STR;
const bool BitCode::CONS=false;
const char BitCode::CONS_CHR=BitCode::CONS?TRUE_CHR:FALSE_CHR;
const string BitCode::CONS_STR=BitCode::CONS?TRUE_STR:FALSE_STR;
const bool BitCode::NIL=true;
const char BitCode::NIL_CHR=BitCode::NIL?TRUE_CHR:FALSE_CHR;
const string BitCode::NIL_STR=BitCode::NIL?TRUE_STR:FALSE_STR;

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
