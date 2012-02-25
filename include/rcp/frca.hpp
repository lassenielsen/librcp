#ifndef RCP_FRCA
#define RCP_FRCA

#include <vector>
#include <set>
#include <rcp/re.hpp>
#include <rcp/bitcode.hpp>

/** worklistitem represents a worklist item
  * for the table filling procedure.
  * Each worklist item represents a position
  * that has ben marked, but has not been checked
  * if it results in more markings.
  */
struct worklistitem
{ int pos;
  bool productive;
};

/** FRCA tags each node with the suffixes
  * of the string it is created by.
  * The FRCA class is a superclass of all
  * the Tagged RE constructor classes,
  * but at the same time represents
  * the regular expression 0.
  * Basically this method is backtracking
  * combined with dynamic programming
  */
class FRCA // Represents 0 {{{
{ public:
    // Create a FRCA which can parse a string of length size
    FRCA();
    virtual ~FRCA();
    virtual std::string ToString() const;
    static FRCA *Create(const RE *exp);
    bool Accept(const std::string &s);
    BitCode CompressGL(const std::string &s);
    BitCode CompressLL(const std::string &s);

    virtual FRCA *Copy() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    // Manage position marks // Returns all new markings produced by the given position
    virtual std::vector<worklistitem> MarkSuffix(const std::string &s, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &s, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();
    // Accessors
    bool HasPrefix(int pos, bool onlyProductive) const;
    bool HasSuffix(int pos, bool onlyProductive) const;
    bool SetPrefix(int pos, bool productive);
    bool SetSuffix(int pos, bool productive);
    std::set<int> GetSuffixes() const;
    std::set<int> GetPrefixes() const;
    int MaxSuffix() const;
    int MaxPrefix() const;

  protected:
  // not private: used by ToString of derived classes
    std::set<int> myPrefixes;
    std::set<int> myProductivePrefixes;
    std::set<int> mySuffixes;
    std::set<int> myProductiveSuffixes;
}; // }}}
class FRCA_One : public FRCA // Represents 1 {{{
{ public:
    FRCA_One();
    virtual ~FRCA_One();
    
    virtual std::string ToString() const;
    virtual FRCA_One *Copy() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
}; // }}}
class FRCA_Char : public FRCA // Represents a {{{
{ public:
    FRCA_Char(char ch);
    virtual ~FRCA_Char();

    virtual std::string ToString() const;
    virtual FRCA_Char *Copy() const;

    char GetChar() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);

  protected:
    char myChar;
}; // }}}
class FRCA_Seq : public FRCA // Represents R1xR2 {{{
{ public:
    FRCA_Seq(FRCA *left, FRCA *right);
    virtual ~FRCA_Seq();

    virtual std::string ToString() const;
    virtual FRCA_Seq *Copy() const;

    const FRCA &GetFront() const;
    const FRCA &GetBack() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();

  protected:
    FRCA *myLeft;
    FRCA *myRight;
}; // }}}
class FRCA_Sum : public FRCA // Represents R1+R2 {{{
{ public:
    FRCA_Sum(FRCA *left, FRCA *right);
    virtual ~FRCA_Sum();

    virtual std::string ToString() const;
    virtual FRCA_Sum *Copy() const;

    const FRCA &GetLeft() const;
    const FRCA &GetRight() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();

  protected:
    FRCA *myLeft;
    FRCA *myRight;
}; // }}}
class FRCA_Star : public FRCA // Represents R1* {{{
{ public:
    FRCA_Star(FRCA *sub);
    virtual ~FRCA_Star();

    virtual std::string ToString() const;
    virtual FRCA_Star *Copy() const;

    const FRCA &GetSub() const;

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual void CompressLL(const std::string &s, int pos1, int pos2, BitCode &dest);
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();

  protected:
    FRCA *mySub;
}; // }}}

#endif
