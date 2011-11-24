#ifndef RCP_BITCODE
#define RCP_BITCODE

#include <vector>
#include <string>

/** BitCode is a class used to represent compact bitvalues
  * condensed into char values for space efficiency.
  */  
class BitCode // {{{
{
  public:
    BitCode();
    virtual ~BitCode();
    unsigned int GetLength() const;
    void PushBit(bool bit);
    bool GetBit(unsigned int index) const;
    std::string ToString() const;

  private:
    std::vector<char> myBits;
    unsigned int myLength;
}; // }}}

#endif
