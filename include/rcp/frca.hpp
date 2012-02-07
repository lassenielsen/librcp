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
    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    static FRCA *Create(const RE *exp, int size);
    // AddSuffix adds a suffix, and returns all new maches produced by that suffix
    virtual std::vector<worklistitem> AddSuffix(const std::string &s, int pos, bool productive);
    // Accessor
    bool HasSuffix(int pos, bool onlyProductive) const;
    int MaxSuffix() const;

  protected:
    std::set<int> mySuffixes;
    std::set<int> myProductiveSuffixes;
}; // }}}
class FRCA_One : public FRCA // Represents 1 {{{
{ public:
    FRCA_One();
    virtual ~FRCA_One();
    
    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> AddSuffix(const std::string &string, int pos, bool productive);
}; // }}}
class FRCA_Char : public FRCA // Represents a {{{
{ public:
    FRCA_Char(char ch);
    virtual ~FRCA_Char();

    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> AddSuffix(const std::string &string, int pos, bool productive);

    char GetChar() const;

  private:
    char myChar;
}; // }}}
class FRCA_Seq : public FRCA // Represents R1xR2 {{{
{ public:
    FRCA_Seq(FRCA *left, FRCA *right);
    virtual ~FRCA_Seq();

    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> AddSuffix(const std::string &string, int pos, bool productive);

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

    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> AddSuffix(const std::string &string, int pos, bool productive);

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

    virtual BitCode Compress(int &pos, bool productive=false) const;
    virtual std::string ToString() const;
    virtual std::vector<worklistitem> AddSuffix(const std::string &string, int pos, bool productive);

    const FRCA &GetSub() const;

  private:
    FRCA *mySub;
}; // }}}

#endif
