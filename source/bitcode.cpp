#include <rcp/bitcode.hpp>
using namespace std;

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
void BitCode::PushBit(bool bit) // {{{
{ if (myLength%8==0)
    myBits.push_back('\0');
  if (bit)
    myBits.back()=myBits.back()|(1<<(myLength%8));
  ++myLength;
  return;
} // }}}
bool BitCode::GetBit(unsigned int index) const // {{{
{ if (index>=myLength)
    throw (string)"BitCode::GetBit index out of bounds";
  return (myBits[index/8])&(1<<(index%8));
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
