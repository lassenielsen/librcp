#ifndef RCP_RE
#define RCP_RE

#include <map>
#include <string>
#include <ostream>
#include <rcp/bitcode.hpp>


/** RV is an abstract superclass for all Regular Expression(RE) values.
  * RE Values is a parse-tree for string values with respect to a regular expressions.
  * Thus it expresses the string value, but also holds information about what
  * part of the regular expression matches each part of the string.
  */  
class RV // {{{
{
  public:
    virtual ~RV() {}
    virtual std::string Flatten() const=0;
    virtual std::string ToString() const=0;
    virtual BitCode BitRep() const=0;
}; // }}}
class RV_Unit : public RV // {{{
{
  public:
    RV_Unit();
    virtual ~RV_Unit();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;
}; // }}}
class RV_Char : public RV // {{{
{
  public:
    RV_Char(char ch);
    virtual ~RV_Char();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;

  private:
    char myChar;
}; // }}}
class RV_Inl : public RV // {{{
{
  public:
    RV_Inl(RV *val);
    virtual ~RV_Inl();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;
    const RV *GetLeft() const;

  private:
    RV *myVal;
}; // }}}
class RV_Inr : public RV // {{{
{
  public:
    RV_Inr(RV *val);
    virtual ~RV_Inr();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;
    const RV *GetRight() const;

  private:
    RV *myVal;
}; // }}}
class RV_Pair : public RV // {{{
{
  public:
    RV_Pair(RV *front, RV *back);
    virtual ~RV_Pair();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;
    const RV *GetFront() const;
    const RV *GetBack() const;

  private:
    RV *myFront;
    RV *myBack;
}; // }}}
class RV_Fold : public RV // {{{
{
  public:
    RV_Fold(RV *val);
    virtual ~RV_Fold();
    virtual std::string Flatten() const;
    virtual std::string ToString() const;
    virtual BitCode BitRep() const;
    const RV *GetSub() const;

  private:
    RV *myVal;
}; // }}}

/** RE is a superclass for all Regular Expressions,
  * but at the same time represents
  * the regular expression 0.
  */
class RE                    // Represents 0 {{{
{ public:
    RE();
    virtual ~RE();
    /** Levels for ToString:
      * 0 = () or init
      * 1 = plus level
      * 2 = seq level
      * 3 = star level
      */
    virtual std::string ToString(int level=0) const; 
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    BitCode Compress(const std::string &s) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;

    virtual RE* Copy() const;
    static RE *Create(const std::string &exp);
  private:
}; // }}}
class RE_One : public RE    // Represents 1 {{{
{ public:
    RE_One();
    virtual ~RE_One();
    virtual std::string ToString(int level=0) const;
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;
    virtual RE_One* Copy() const;

  private:
}; // }}}
class RE_Char : public RE   // Represents a {{{
{ public:
    RE_Char(char ch);
    virtual ~RE_Char();
    virtual std::string ToString(int level=0) const;
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;
    virtual RE_Char* Copy() const;
    char GetChar() const;

  private:
    char myChar;
}; // }}}
class RE_Seq : public RE    // Represents R1xR2 {{{
{ public:
    RE_Seq(RE *left, RE *right);
    virtual ~RE_Seq();
    virtual std::string ToString(int level=0) const;
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;
    virtual RE_Seq* Copy() const;
    const RE &GetFront() const;
    const RE &GetBack() const;

  private:
    RE *myLeft;
    RE *myRight;
}; // }}}
class RE_Sum : public RE    // Represents R1+R2 {{{
{ public:
    RE_Sum(RE *left, RE *right);
    virtual ~RE_Sum();
    virtual std::string ToString(int level=0) const;
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;
    virtual RE_Sum* Copy() const;
    const RE &GetLeft() const;
    const RE &GetRight() const;

  private:
    RE *myLeft;
    RE *myRight;
}; // }}}
class RE_Star : public RE   // Represents R1* {{{
{ public:
    RE_Star(RE *sub);
    virtual ~RE_Star();
    virtual std::string ToString(int level=0) const;
    virtual RV* Decompress(const BitCode &bitvalue, int &pos) const;
    virtual void Decode(const BitCode &bitvalue, int &pos, std::ostream &dest) const;
    virtual bool Accept(const std::string &s, int start=0, int end=-1) const;
    virtual BitCode Compress(const std::string &s, int start, int end, bool &accept) const;
    virtual RE_Star* Copy() const;
    const RE &GetSub() const;

  private:
    RE *mySub;
}; // }}}

#endif
