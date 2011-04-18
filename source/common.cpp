#include <rcp/common.hpp>

bool is_prefix(const std::string &a, const std::string &b) // {{{
{
  return a==b.substr(0,a.size());
} // }}}
