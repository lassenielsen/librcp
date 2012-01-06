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
    bool GetBit(unsigned int index) const;

    void PushBit(bool bit);
    void PushBit(char val);
    void Append(const BitCode &rhs);
    void Append(const std::string &rhs);
    BitCode Reverse();

    std::string ToString() const;

  private:
    std::vector<char> myBits;
    unsigned int myLength;
}; // }}}

/** Define codes for the non-trivial constructors
  */
const bool BC_INL = false;
const char BC_INL_CHR = '0';
const std::string BC_INL_STR = "0";
const char BC_INR = true;
const char BC_INR_CHR = '1';
const std::string BC_INR_STR = "1";
const char BC_CONS = false;
const char BC_CONS_CHR = '0';
const std::string BC_CONS_STR = "0";
const char BC_NIL = true;
const char BC_NIL_CHR = '1';
const std::string BC_NIL_STR = "1";

#endif
