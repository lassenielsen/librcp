#ifndef RCP_FRCA
#define RCP_FRCA

#include <rcp/re.hpp>
#include <vector>

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
    FRCA(int size);
    virtual ~FRCA();
    bool Accept() const;
    virtual std::string Compress(int &pos) const;
    virtual std::string ToString() const;
    static FRCA *Create(const RE *exp, int size);
    // AddSuffix adds a suffix, and returns all new maches produced by that suffix
    virtual std::vector<int> AddSuffix(const std::string &s, int pos);
    // Accessor
    bool HasSuffix(int pos) const;
    int MaxSuffix() const;

  protected:
    bool *mySuffixes;
    int mySize;
}; // }}}
class FRCA_One : public FRCA // Represents 1 {{{
{ public:
    FRCA_One(int size);
    virtual ~FRCA_One();
    
    virtual std::string Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);
}; // }}}
class FRCA_Char : public FRCA // Represents a {{{
{ public:
    FRCA_Char(int size, char ch);
    virtual ~FRCA_Char();

    virtual std::string Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

    char GetChar() const;

  private:
    char myChar;
}; // }}}
class FRCA_Seq : public FRCA // Represents R1xR2 {{{
{ public:
    FRCA_Seq(int size, FRCA *left, FRCA *right);
    virtual ~FRCA_Seq();

    virtual std::string Compress(int &pos) const;
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
    FRCA_Sum(int size, FRCA *left, FRCA *right);
    virtual ~FRCA_Sum();

    virtual std::string Compress(int &pos) const;
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
    FRCA_Star(int size, FRCA *sub);
    virtual ~FRCA_Star();

    virtual std::string Compress(int &pos) const;
    virtual std::string ToString() const;
    virtual std::vector<int> AddSuffix(const std::string &string, int pos);

    const FRCA &GetSub() const;

  private:
    FRCA *mySub;
}; // }}}

#endif
