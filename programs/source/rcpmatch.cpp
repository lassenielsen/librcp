#include <rcp/re.hpp>
#include <rcp/nfa.hpp>
#include <rcp/dfa.hpp>
#include <rcp/frca.hpp>
#include <iostream>
#include <sys/timeb.h>
#include <sstream>

using namespace std;

string recstar_re(int n) // {{{
{
  string result="";
  for (int i=0;i<n;++i)
    result +="(";
  result += "a";
  for (int i=0;i<n;++i)
    result +=")*a";
  return result;
} // }}}
string cox_re(int n) // {{{
{
  string result="";
  for (int i=0;i<n;++i)
    result +="(1+a)";
  for (int i=0;i<n;++i)
    result +="a";
  return result;
} // }}}
string cox_reinv(int n) // {{{
{
  string result="";
  for (int i=0;i<n;++i)
    result +="a";
  for (int i=0;i<n;++i)
    result +="(1+a)";
  return result;
} // }}}
string repeat_a(int n) // {{{
{
  return string(n,'a');
} // }}}
string dexp_re(int n) // {{{
{
  string result="(a+b)*a";
  for (int i=0;i<n;++i)
    result +="(a+b)";
  return result;
} // }}}
string str_url(int n) // {{{
{ stringstream ss;
  ss << "http://";
  for (int i=0; i<n; ++i)
  { for (int j=0; j<i%27+3; ++j)
      ss << (char)('a'+(j%25));
    ss << ".";
  }
  ss << "com";
  return ss.str();
} // }}}
string str_email(int n) // {{{
{ stringstream ss;
  for (int i=0; i<n; ++i)
  { for (int j=0; j<i%27+3; ++j)
      ss << (char)('a'+(j%26));
    if (n>i+1)
      ss << "-";
  }
  ss << "@";
  for (int j=0; j<2; ++j)
  { for (int i=0; i<n+3; ++i)
      ss << (char)('a'+((i+j)%26));
    ss << ".";
  }
  ss << "com";
  return ss.str();
} // }}}
string str_amount(int n) // {{{
{ stringstream ss;
  ss << "$";
  for (int i=0; i<n; ++i)
  {
    ss << (1+i)%10;
    if (n>i+1 && (n-i)%3==1)
      ss << ",";
  }
  ss << ".53";
  return ss.str();
} // }}}
string str_float(int n) // {{{
{ stringstream ss;
  ss << "-";
  for (int i=0; i<n; ++i)
  {
    ss << (1+i)%10;
  }
  ss << ".";
  for (int i=0; i<n; ++i)
  {
    ss << (1+i)%10;
  }
  ss << "E+";
  for (int i=0; i<n; ++i)
  {
    ss << (1+i)%10;
  }
  return ss.str();
} // }}}

double gettime() // {{{
{ struct timeb tb;
  ftime(&tb);
  double result = tb.millitm;
  result /= 1000;
  result+= (tb.time % (60*60));
  return result;
} // }}}

int main(int argc, char **argv)
{
  int n=-1;
  int t=-1;
  string re_method="help";
  // Parse input {{{
  if (argc>=2)
  { re_method=argv[1];
  }
  if (re_method=="help")
  { cout << "Syntax: " << argv[0] << " [help|re|nfa|nfa_reduce|nfa_minimize|nfa_thompson|dfa|dfa_reduce|dfa_minimize|dfasim|dfasim_reduce|frca|debug] [print|accept|decompress|compress|compile|time|timems|size] [<exp> [<str>]]" << endl;
    return 0;
  }
  string re_cmd="frca";
  if (argc>=3)
  { re_cmd=argv[2];
  }
  string re_exp="a*b*a*";
  if (argc>=4) // {{{
  { re_exp=argv[3];
    if (!re_exp.compare(0, 3, "cox"))
    { stringstream ss;
      ss << re_exp.substr(3);
      ss >> t;
      re_exp=cox_re(t);
      if (re_cmd=="time")
        cout << "# Exp: COX t=" << t << endl;
    }
    else if (!re_exp.compare(0, 6, "invcox"))
    { stringstream ss;
      ss << re_exp.substr(6);
      ss >> t;
      re_exp=cox_reinv(t);
      if (re_cmd=="time")
        cout << "# Exp: COX t=" << t << endl;
    }
    else if (!re_exp.compare(0, 4, "dexp"))
    { stringstream ss;
      ss << re_exp.substr(4);
      ss >> t;
      re_exp=dexp_re(t);
      if (re_cmd=="time")
        cout << "# Exp: DEXP t=" << t << endl;
    }
    else if (!re_exp.compare(0, 7, "recstar"))
    { stringstream ss;
      ss << re_exp.substr(7);
      ss >> t;
      re_exp=recstar_re(t);
      if (re_cmd=="time")
        cout << "# Exp: RECSTAR t=" << t << endl;
    }
  } // }}}
  string re_str="";
  if (argc>=5) // {{{
  { re_str=argv[4];
    if (!re_str.compare(0, 3, "cox"))
    { stringstream ss;
      ss << re_str.substr(3);
      ss >> n;
      re_str=repeat_a(n);
      if (re_cmd=="time")
        cout << "# Str: a^" << n << endl;
    }
    if (!re_str.compare(0, 4, "dexp"))
    { stringstream ss;
      ss << re_str.substr(4);
      ss >> n;
      re_str=repeat_a(n+1);
      if (re_cmd=="time")
        cout << "# Str: DEXP n=" << n << endl;
    }
    if (!re_str.compare(0, 7, "recstar"))
    { stringstream ss;
      ss << re_str.substr(7);
      ss >> n;
      re_str=repeat_a(n) + "b" + repeat_a(n);
      if (re_cmd=="time")
        cout << "# Str: RECSTAR n=" << n << endl;
    }
    if (!re_str.compare(0, 3, "url"))
    { stringstream ss;
      ss << re_str.substr(3);
      ss >> n;
      re_str=str_url(n);
      if (re_cmd=="time")
        cout << "# Str: URL n=" << n << endl;
    }
    if (!re_str.compare(0, 5, "email"))
    { stringstream ss;
      ss << re_str.substr(5);
      ss >> n;
      re_str=str_email(n);
      if (re_cmd=="time")
        cout << "# Str: EMAIL n=" << n << endl;
    }
    if (!re_str.compare(0, 6, "amount"))
    { stringstream ss;
      ss << re_str.substr(6);
      ss >> n;
      re_str=str_amount(n);
      if (re_cmd=="time")
        cout << "# Str: AMOUNT n=" << n << endl;
    }
    if (!re_str.compare(0, 5, "float"))
    { stringstream ss;
      ss << re_str.substr(5);
      ss >> n;
      re_str=str_float(n);
      if (re_cmd=="time")
        cout << "# Str: FLOAT n=" << n << endl;
    }
  } // }}}
  // }}}
  if (re_cmd=="debug" || re_method=="debug")
  { cout << "Exp: " << re_exp << endl;
    cout << "Str: " << re_str << endl;
    return 0;
  }
  RE *re=RE::Create(re_exp);
  if (re_method=="re") // {{{
  { if (re_cmd=="print") // {{{
    { cout << re->ToString() << endl;
      return 0;
    } // }}}
    if (re_cmd=="decompress") // {{{
    { int pos=0;
      cout << re->Decompress(re_str,pos)->Flatten() << endl;
      if (pos != re_str.size())
        cout << "Error: Not alle of the bitvalue was used" << endl;
      return 0;
    } // }}}
    else if (re_cmd=="accept")
    { cout << re->Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << re->Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      re->Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown NFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="nfa") // {{{
  { double c_1=gettime();
    NFA nfa(*re);
    double c_2=gettime();
    if (re_cmd=="print")
    { cout << nfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << nfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << nfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      nfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "NFA Nodes: " << nfa.CountNodes() << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="nfa_minimize") // {{{
  { double c_1=gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.MakePrefix();
    nfa.Reduce();
    double c_2=gettime();
    if (re_cmd=="print")
    { cout << nfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << nfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << nfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      nfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "NFA Nodes: " << nfa.CountNodes() << endl;
      return 0;
    }
    else
    { cout << "Unknown NFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="nfa_reduce") // {{{
  { double c_1=gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.Reduce();
    nfa.Explode();
    double c_2=gettime();
    if (re_cmd=="print")
    { cout << nfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << nfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << nfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      nfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "NFA Nodes: " << nfa.CountNodes() << endl;
      return 0;
    }
    else
    { cout << "Unknown NFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="nfa_thompson") // {{{
  { double c_1=gettime();
    NFA nfa(*re);
    double c_2=gettime();
    if (re_cmd=="print")
    { cout << nfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { try
      { pair<unsigned int, vector<char> > result=nfa.ThompsonGL(re_str);
        cout << "Number of bits: " << result.first << endl;
      }
      catch (string s)
      {
        cout << "Error: " << s << endl;
      }
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      nfa.ThompsonGL(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown NFA_THOMPSON command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfa") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    DFA dfa(nfa);
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compile")
    { cout << dfa.Compile() << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="runtime")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "DFA Nodes: " << dfa.CountNodes() << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfa_reduce") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.Reduce();
    nfa.Explode();
    DFA dfa(nfa);
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compile")
    { cout << dfa.Compile() << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="runtime")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "DFA Nodes: " << dfa.CountNodes() << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfa_minimize") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.MakePrefix();
    nfa.Reduce();
    nfa.Explode();
    DFA dfa(nfa);
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compile")
    { cout << dfa.Compile() << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="runtime")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << t_2-t_1 << endl;
      return 0;
    }
    else if (re_cmd=="size")
    { cout << "DFA Nodes: " << dfa.CountNodes() << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfasim") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    DFA dfa(nfa,false); // Create DFA structure, without precomputing DFA states
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfasim_reduce") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.Reduce();
    nfa.Explode();
    DFA dfa(nfa,false); // Create DFA structure, without precomputing DFA states
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="dfasim_minimize") // {{{
  { double c_1 = gettime();
    NFA nfa(*re);
    nfa.RemoveDeadStates();
    nfa.MakeCompact();
    nfa.RemoveUnreachableStates();
    nfa.MakePrefix();
    nfa.Reduce();
    nfa.Explode();
    DFA dfa(nfa,false); // Create DFA structure, without precomputing DFA states
    double c_2 = gettime();
    if (re_cmd=="print")
    { cout << dfa.ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << dfa.Accept(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { cout << dfa.Compress(re_str) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      dfa.Compress(re_str);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown DFA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else if (re_method=="frca") // {{{
  { double c_1=gettime();
    FRCA *frca=FRCA::Create(re, re_str.size());
    frca->AddSuffix(re_str,re_str.size());
    double c_2=gettime();
    if (re_cmd=="print")
    { cout << frca->ToString() << endl;
      return 0;
    }
    else if (re_cmd=="accept")
    { cout << frca->Accept() << endl;
      return 0;
    }
    else if (re_cmd=="compress")
    { int pos=0;
      cout << frca->Compress(pos) << endl;
      return 0;
    }
    else if (re_cmd=="time")
    { double t_1=gettime();
      int pos=0;
      frca->Compress(pos);
      double t_2=gettime();
      if (n>=0)
        cout << n << " ";
      cout.precision(10);
      cout << c_2-c_1+t_2-t_1 << endl;
      return 0;
    }
    else
    { cout << "Unknown FRCA command: " << re_cmd << endl;
      return 0;
    }
  } // }}}
  else
  { cout << "Unknown command: " << re_cmd << ", use help for possibilities" << endl;
    return 0;
  }
}
