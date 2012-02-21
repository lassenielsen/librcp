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
    bool Accept() const;
    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    static FRCA *Create(const RE *exp);
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

    virtual FRCA *Copy() const;

  protected:
    std::set<int> myPrefixes;
    std::set<int> myProductivePrefixes;
    std::set<int> mySuffixes;
    std::set<int> myProductiveSuffixes;
}; // }}}
class FRCA_One : public FRCA // Represents 1 {{{
{ public:
    FRCA_One();
    virtual ~FRCA_One();
    
    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual FRCA_One *Copy() const;
}; // }}}
class FRCA_Char : public FRCA // Represents a {{{
{ public:
    FRCA_Char(char ch);
    virtual ~FRCA_Char();

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual FRCA_Char *Copy() const;

    char GetChar() const;

  private:
    char myChar;
}; // }}}
class FRCA_Seq : public FRCA // Represents R1xR2 {{{
{ public:
    FRCA_Seq(FRCA *left, FRCA *right);
    virtual ~FRCA_Seq();

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();
    virtual FRCA_Seq *Copy() const;

    const FRCA &GetFront() const;
    const FRCA &GetBack() const;

  private:
    FRCA *myLeft;
    FRCA *myRight;
}; // }}}
class FRCA_Sum : public FRCA // Represents R1+R2 {{{
{ public:
    FRCA_Sum(FRCA *left, FRCA *right);
    virtual ~FRCA_Sum();

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();
    virtual FRCA_Sum *Copy() const;

    const FRCA &GetLeft() const;
    const FRCA &GetRight() const;

  private:
    FRCA *myLeft;
    FRCA *myRight;
}; // }}}
class FRCA_Star : public FRCA // Represents R1* {{{
{ public:
    FRCA_Star(FRCA *sub);
    virtual ~FRCA_Star();

    virtual BitCode CompressGL(int &pos, bool productive=false) const;
    virtual BitCode CompressLL(const std::string &s, int pos1, int pos2);
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> MarkSuffix(const std::string &string, int pos, bool productive);
    virtual std::vector<worklistitem> MarkPrefix(const std::string &string, int pos, bool productive);
    virtual void ClearSuffixes();
    virtual void ClearPrefixes();
    virtual FRCA_Star *Copy() const;

    const FRCA &GetSub() const;

  private:
    FRCA *mySub;
}; // }}}

#endif
