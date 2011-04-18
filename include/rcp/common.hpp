#ifndef RCP_COMMON
#define RCP_COMMON

#include <string>
#include <vector>
#include <list>

bool is_prefix(const std::string &a, const std::string &b);

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
#endif
