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
    void Append(const BitCode &rhs);
    void Append(const std::string &rhs);
    BitCode Reverse();

    std::string ToString() const;

    static const bool INL;
    static const char INL_CHR;
    static const std::string INL_STR;
    static const bool INR;
    static const char INR_CHR;
    static const std::string INR_STR;
    static const bool CONS;
    static const char CONS_CHR;
    static const std::string CONS_STR;
    static const bool NIL;
    static const char NIL_CHR;
    static const std::string NIL_STR;

  private:
    std::vector<char> myBits;
    unsigned int myLength;
}; // }}}

/** Define codes for the non-trivial constructors
  */
#endif
