#ifndef RCP_FRCA
#define RCP_FRCA

#include <vector>
#include <set>
#include <rcp/re.hpp>
#include <rcp/bitcode.hpp>

/** FRCA uses tags each node with the suffixes
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
    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    static FRCA *Create(const RE *exp, int size);
    // AddSuffix adds a suffix, and returns all new maches produced by that suffix
    virtual std::vector<int> AddSuffix(const std::string &s, int pos);
    // Accessor
    bool HasSuffix(int pos) const;
    int MaxSuffix() const;

  protected:
    std::set<int> mySuffixes;
}; // }}}
class FRCA_One : public FRCA // Represents 1 {{{
{ public:
    FRCA_One();
    virtual ~FRCA_One();
    
    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);
}; // }}}
class FRCA_Char : public FRCA // Represents a {{{
{ public:
    FRCA_Char(char ch);
    virtual ~FRCA_Char();

    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

    char GetChar() const;

  private:
    char myChar;
}; // }}}
class FRCA_Seq : public FRCA // Represents R1xR2 {{{
{ public:
    FRCA_Seq(FRCA *left, FRCA *right);
    virtual ~FRCA_Seq();

    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

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

    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

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

    virtual BitCode Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

    const FRCA &GetSub() const;

  private:
    FRCA *mySub;
}; // }}}

#endif
