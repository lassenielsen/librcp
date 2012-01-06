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

    static bool INL;
    static char INL_CHR;
    static std::string INL_STR;
    static bool INR;
    static char INR_CHR;
    static std::string INR_STR;
    static bool CONS;
    static char CONS_CHR;
    static std::string CONS_STR;
    static bool NIL;
    static char NIL_CHR;
    static std::string NIL_STR;

  private:
    std::vector<char> myBits;
    unsigned int myLength;
}; // }}}

/** Define codes for the non-trivial constructors
  */
#endif
