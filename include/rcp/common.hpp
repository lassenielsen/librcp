#ifndef RCP_COMMON
#define RCP_COMMON

#include <string>
#include <vector>
#include <list>
#include <utility>

inline bool is_prefix(const std::string &a, const std::string &b) // {{{
{
  return a==b.substr(0,a.size());
} // }}}

inline std::pair<int,int> index2pos(const std::string &str, int index) // {{{
{ int line=1;
  int column=1;
  for (int i=0; i<str.size() && i<index; ++i)
    if (str[i]=='\n')
    { ++line;
      column=1;
    }
    else
      ++column;

  return std::pair<int,int>(line,column);
} // }}}

template <class T>
void DeleteList(std::list<T*> &container) // {{{
{
  while (container.size() > 0)
  {
    delete container.back();
    container.pop_back();
  }
} // }}}
template <typename T>
void CopyList(const std::list<T*> &source, std::list<T*> &dest) // {{{
{
  for (typename std::list<T*>::const_iterator it=source.begin(); it!=source.end(); ++it)
    dest.push_back(new T(*(*it)));
} // }}}
template <class T>
void MoveList(std::list<T*> &source, std::list<T*> &dest) // {{{
{
  while (source.size()>0)
  { dest.push_back(source.front());
    source.pop_front();
  }
} // }}}

inline std::string linecol(const std::string &s, int pos) // {{{
{ if (pos > s.size())
    return "index out of data";

  int line=1;
  int col=1;
  for (int i=0; i<pos; ++i)
  { if (s[i]=='\n')
    { ++line;
      col=1;
    }
    else
      ++col;
  }
  return std::string("line: ") + std::to_string(line) + ", col: " + std::to_string(col);
} // }}}
#endif
