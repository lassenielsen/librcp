#include <rcp/dfa.hpp>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <rcp/common.hpp>
using namespace std;
using namespace __gnu_cxx;
DFATransition::DFATransition() // {{{
: myDest(-1),
  myOutput()
{
} // }}}
DFATransition::DFATransition(int dest, const map<int,pair<string,int> > &output) // {{{
: myDest(dest),
  myOutput(output)
{
} // }}}
DFATransition::~DFATransition() // {{{
{
} // }}}
int DFATransition::GetDest() const // {{{
{
  return myDest;
} // }}}
const string _emptystring="";
const string &DFATransition::GetOutput(int dest, int &source) const // {{{
{
  map<int,pair<string,int> >::const_iterator it=myOutput.find(dest);
  if (it==myOutput.end())
  { source=-1;
    return _emptystring;
  }
  source = it->second.second;
  return it->second.first;
} // }}}
string DFATransition::ToString(int source, char input) const // {{{
{
  stringstream result;
  result << "\n " << "edge_" << source << "_" << GetDest() << "_" << input << " [shape=record, label=\"";
  for (map<int,pair<string,int> >::const_iterator dest=myOutput.begin(); dest!=myOutput.end(); ++dest)
  {
    if (dest!=myOutput.begin())
      result << " | ";
    result << dest->first << ":=" << dest->second.second << "+" << dest->second.first;
  }
  result << "\"]\n " << source << "->" << "edge_" << source << "_" << GetDest() << "_" << input
         << " [label=\"" << input << "\"]"
         << "\n edge_" << source << "_" << GetDest() << "_" << input << "->" << GetDest();
  return result.str();
} // }}}

DFANode::DFANode(const set<int> &nfanodes) // {{{
: myFinal(-1),
  myNfaNodes(nfanodes)
{
  myTransitions.clear();
} // }}}
DFANode::~DFANode() // {{{
{
} // }}}
int DFANode::Final() const // {{{
{
  return myFinal;
} // }}}
void DFANode::SetFinal(int final) // {{{
{
  myFinal=final;
} // }}}
void DFANode::AddTransition(char input, const DFATransition &trans) // {{{
{
  myTransitions[input] = trans;
} // }}}
//const vector<DFATransition> &DFANode::GetTransitions() const // {{{
//{
//  return myTransitions;
//} // }}}
//int DFANode::CountTransitions() const // {{{
//{
//  return myTransitions.size();
//} // }}}
set<char> DFANode::GetEdges() const // {{{
{
  set<char> result;
  for (hash_map<char, DFATransition>::const_iterator it=myTransitions.begin(); it!=myTransitions.end(); ++it)
  { if (it->second.GetDest()>=0)
      result.insert(it->first);
  }
  return result;
} // }}}
const DFATransition __empty_transition;
const DFATransition &DFANode::GetTransition(char input) const // {{{
{
  hash_map<char, DFATransition>::const_iterator it=myTransitions.find(input);
  if (it!=myTransitions.end())
    return it->second;
  return __empty_transition;
} // }}}
const set<int> &DFANode::GetNFANodes() const // {{{
{
  return myNfaNodes;
} // }}}
// int DFANode::FindSupremum(const string &max) const // {{{
// {
//   int b=0;
//   int t=myTransitions.size()-1;
//   while (t-b>10)
//   { int mid = (t+b+1)/2;
//     if (myTransitions[mid].GetInput()<=max)
//       b=mid;
//     else
//       t=mid-1;
//   }
//   while (b<=t && myTransitions[t].GetInput()>max)
//     --t;
//   return t;
// } // }}}
string DFANode::ToString(int id) const // {{{
{
  stringstream result;
  result << "\n " << id << " [label=\"" << id << ": ";
  for (set<int>::const_iterator nfanode=myNfaNodes.begin(); nfanode!=myNfaNodes.end(); ++nfanode)
  {
    if (nfanode!=myNfaNodes.begin())
      result << ",";
    if (*nfanode==Final())
      result << "<" << *nfanode << ">";
    else
      result << *nfanode;
  }
  result << "\"";
  if (Final()>=0)
    result << ", shape=doublecircle";
  else
    result << ", shape=circle";
  result << "]";
  for (hash_map<char, DFATransition>::const_iterator it=myTransitions.begin(); it!=myTransitions.end(); ++it)
    result << it->second.ToString(id,it->first);
  return result.str();
} // }}}

DFA::DFA(const NFA &nfa, bool precompile) // {{{
: myNfa(nfa)
{
  // Find first DFA node as the epsilon-closure of the first NFA node
  map<int,pair<string,int> > initCodes;
  initCodes[0].first="";
  initCodes[0].second=0;
  nfa.Closure(initCodes);
  set<int> init;
  // Save initial bitcodes
  for (map<int,pair<string,int> >::const_iterator code=initCodes.begin(); code!=initCodes.end(); ++code)
  {
    myInitCodes[code->first]=code->second.first;
    init.insert(code->first);
  }
  int dfanode=AddNode(init);
  for (set<int>::const_iterator nfanode=init.begin(); nfanode!=init.end(); ++nfanode)
    if (nfa.GetNode(*nfanode).Final())
      myNodes[dfanode].SetFinal(*nfanode);
  // Create the remaining DFA edges and nodes
  if (precompile)
    Create(dfanode);
} // }}}
DFA::~DFA() // {{{
{
} // }}}
int DFA::AddNode(const set<int> &nfanodes) // {{{
{
  for (int i=0; i<myNodes.size(); ++i)
    if (myNodes[i].GetNFANodes()==nfanodes)
      return i;
  // If nodes not found
  myNodes.push_back(DFANode(nfanodes));
  // And select final state, if any
  for (set<int>::const_iterator nfanode=nfanodes.begin(); nfanode!=nfanodes.end(); ++nfanode)
    if (myNfa.GetNode(*nfanode).Final())
      myNodes.back().SetFinal(*nfanode);
  return myNodes.size()-1;
} // }}}
int DFA::CountNodes() // {{{
{ return myNodes.size();
} // }}}
bool DFA::Accept(const string &s) // {{{
{
  int node=0; // We start in node 0
  for (int pos=0; pos<s.size() && node>=0; ++pos)
  { const DFATransition &edge=myNodes[node].GetTransition(s[pos]);
    if (edge.GetDest()<0)
      node=CreateNode(node,s[pos]);
    else
      node=edge.GetDest();
  }
  return node>=0 && myNodes[node].Final()>=0;
} // }}}
string DFA::Compress(const string &s) // {{{
{ // Find DFA path
  vector<int> path(s.size()+1);
  path[0]=0;
  for (int pos=0;pos<s.size(); ++pos)
  { path[pos+1]=myNodes[path[pos]].GetTransition(s[pos]).GetDest();
    if (path[pos+1]<0)
    { path[pos+1]=CreateNode(path[pos],s[pos]);
      if (path[pos+1]<0)
        return "No match";
    }
  }
  // Find NFA node by tracing the DFA path backwards from (the) final NFA node in the end state
  vector<int> nfa_node(s.size()+1);
  nfa_node[s.size()] = myNodes[path[s.size()]].Final();
  if (nfa_node[s.size()]<0)
    return "No match";
  for (int pos=s.size(); pos>0; --pos)
    myNodes[path[pos-1]].GetTransition(s[pos-1]).GetOutput(nfa_node[pos],nfa_node[pos-1]);
  stringstream result;
  // Sequence output from NFA path
  map<int,string>::const_iterator init=myInitCodes.find(nfa_node[0]);
  if (init==myInitCodes.end())
    return "No Match";
  result << init->second;
  int prev;
  for (int pos=0; pos<s.size(); ++pos)
    result << myNodes[path[pos]].GetTransition(s[pos]).GetOutput(nfa_node[pos+1],prev);
  return result.str();
} // }}}
string DFA::ToString() const // {{{
{
  stringstream result;
  result << "digraph DFA\n{\n graph [rankdir=\"LR\"]\n start [shape=record, periferies=2, label=\"";
  for (map<int,std::string>::const_iterator it=myInitCodes.begin(); it!=myInitCodes.end(); ++it)
  {
    if (it!=myInitCodes.begin())
      result << " | ";
    result << it->first << ":=" << it->second;
  }
  result << "\"]";
  result << "\n start -> 0";
  for (int i=0; i<myNodes.size(); ++i)
    result << myNodes[i].ToString(i);
  result << "\n}";
  return result.str();
} // }}}
string DFA::Compile() const // {{{
{
  stringstream result;
  Compile(result);
  return result.str();
} // }}}
void DFA::Compile(ostream &dest) const // {{{
{
  dest << "#include<iostream>" << endl
       << "#include<vector>" << endl
       << "#include<list>" << endl
       << "using namespace std;" << endl
       << "int main()" << endl
       << "{ int state=0;" << endl
       << "  int nfa_state;" << endl
       << "  vector< pair<int,char> > path;" << endl
       << "  list<string> output;" << endl
       << "  char ch;" << endl
       << "  while (cin.get(ch) && (int)ch>10)" << endl
       << "  {" << endl
       << "    switch(state)" << endl
       << "    {" << endl;
  for (int i=0; i<myNodes.size(); ++i)
  { dest << "      case " << i << ": // {{{" << endl
         << "        switch (ch)" << endl
         << "        {" << endl;
    set<char> edges=myNodes[i].GetEdges();
    for (set<char>::const_iterator edge=edges.begin(); edge!=edges.end(); ++edge)
      dest << "          case '" << (*edge) << "': // {{{" << endl
           << "            path.push_back(pair<int,char>(state,ch));" << endl
           << "            state=" << myNodes[i].GetTransition(*edge).GetDest() << ";" << endl
           << "            break; // }}}" << endl;
    dest << "          default:" << endl
         << "            cout << \"Error: Mismatch!\" << endl;" << endl
         << "            return -1;" << endl
         << "        }" << endl
         << "        break; // }}}" << endl;
  }
  dest << "      default:" << endl
       << "        cout << \"Error: Unknown state!\" << endl;" << endl
       << "        return -1;" << endl
       << "    }" << endl
       << "  }" << endl
       << "  switch (state) // {{{" << endl
       << "  {" << endl;
  for (int i=0; i<myNodes.size(); ++i)
  { dest << "      case " << i << ":" << endl;
    if (myNodes[i].Final()>0)
      dest << "        //cout << \"String accepted\" << endl;" << endl
           << "        nfa_state=" << myNodes[i].Final() << ";" << endl;
    else
      dest << "        cout << \"String rejected\" << endl;" << endl
           << "        return 0;" << endl;
    dest << "        break;" << endl;
  }
  dest << "    default:" << endl
       << "      cout << \"Error: Unknown end state!\" << endl;" << endl
       << "      return -1;" << endl
       << "  } // }}}" << endl
       << "  while (path.size()>0)" << endl
       << "  { switch (path.back().first)" << endl
       << "    {" << endl;
  for (int i=0; i<myNodes.size(); ++i)
  { dest << "      case " << i << ": // {{{" << endl
         << "        switch (path.back().second)" << endl
         << "        {" << endl;
    set<char> edges=myNodes[i].GetEdges();
    for (set<char>::const_iterator edge=edges.begin(); edge!=edges.end(); ++edge)
    {
      dest << "          case '" << (*edge) << "': // {{{" << endl
           << "            switch(nfa_state)" << endl
           << "            {" << endl;
      for (set<int>::const_iterator k=myNodes[myNodes[i].GetTransition(*edge).GetDest()].GetNFANodes().begin();
                                    k!=myNodes[myNodes[i].GetTransition(*edge).GetDest()].GetNFANodes().end();
                                    ++k)
      { int nfa_prevstate;
        string out = myNodes[i].GetTransition(*edge).GetOutput(*k,nfa_prevstate);
        dest << "              case " << *k << ": // {{{" << endl
             << "                output.push_front(\"" << out << "\");" << endl
             << "                nfa_state=" << nfa_prevstate << ";" << endl
             << "                break; // }}}" << endl;
      }
      dest << "              default:" << endl
           << "                cout << \"Illegal NFA State!\" << endl;" << endl
           << "            }" << endl
           << "            break; // }}}" << endl;
    }
    dest << "          default: " << endl
        << "             cout << \"Illegal Path!\" << endl;" << endl
         << "        }" << endl
         << "        break; // }}}" << endl;
  }
  dest << "    }" << endl
       << "    state=path.back().first;" << endl
       << "    path.pop_back();" << endl
       << "  }" << endl
       << "  switch (nfa_state) // {{{" << endl
       << "  {" << endl;
  for (map<int,string>::const_iterator k=myInitCodes.begin();
       k!=myInitCodes.end();
       ++k)
    dest << "    case " << k->first << ": // {{{" << endl
         << "      output.push_front(\"" << k->second << "\");" << endl
         << "      break; // }}}" << endl;
  dest << "  } // }}}" << endl
       << "  for (list<string>::const_iterator it=output.begin(); it!=output.end(); ++it)" << endl
       << "    cout << *it;" << endl
       << "  cout << endl;" << endl
       << "  return 0;" << endl
       << "}";
} // }}}
void DFA::Create(int pos) // {{{
{
  // Find transitions for each node
  for (;pos<myNodes.size();++pos)
  {
    // Structure to collect edges with same input.
    map<char,map<int,pair<string,int> > > destinations;
    destinations.clear();
    // Find direct edges, and collect same input edges
    for (set<int>::const_iterator it=myNodes[pos].GetNFANodes().begin();
         it!=myNodes[pos].GetNFANodes().end(); ++it)
    for (int i=0; i<myNfa.GetNode(*it).CountTransitions(); ++i)
    {
      if (myNfa.GetNode(*it).GetTransition(i).GetInput()!="") // If not epsilon-edge
      { destinations[myNfa.GetNode(*it).GetTransition(i).GetInput()[0]]
                    [myNfa.GetNode(*it).GetTransition(i).GetDest()]=
          pair<string,int>(myNfa.GetNode(*it).GetTransition(i).GetOutput(),*it);
      }
    }
    for (map<char,map<int,pair<string,int> > >::iterator edge=destinations.begin();
         edge!=destinations.end();
         ++edge)
    {
      // Find closures
      myNfa.Closure(edge->second);
      // Extract NFA nodes
      set<int> dest_nodes;
      for (map<int,pair<string,int> >::const_iterator it=edge->second.begin();
           it!=edge->second.end(); ++it)
        dest_nodes.insert(it->first);
      // Add destination node (if new)
      int dest=AddNode(dest_nodes);
      // Add transition
      myNodes[pos].AddTransition(edge->first,DFATransition(dest,edge->second));
    }
  }
} // }}}
int DFA::CreateNode(int pos, char input) // {{{
{
  // destEdge holds the destination nodes,
  // with their source node and the bitvalue of the used path
  map<int,pair<string,int> > destEdge;

  // Try each edge from the current position
  for (set<int>::const_iterator it=myNodes[pos].GetNFANodes().begin();
       it!=myNodes[pos].GetNFANodes().end(); ++it)
  for (int i=0; i<myNfa.GetNode(*it).CountTransitions(); ++i)
  {
    if (myNfa.GetNode(*it).GetTransition(i).GetInput().size()==1 &&
        myNfa.GetNode(*it).GetTransition(i).GetInput()[0]==input) // If matches input
    { destEdge[myNfa.GetNode(*it).GetTransition(i).GetDest()]=
        pair<string,int>(myNfa.GetNode(*it).GetTransition(i).GetOutput(),*it);
    }
  }
  if (destEdge.size() == 0)
    return -1; // No transition found

  // Find closure of destination nodes
  myNfa.Closure(destEdge);
  // Extract NFA nodes
  set<int> destNodes;
  for (map<int,pair<string,int> >::const_iterator it=destEdge.begin();
       it!=destEdge.end(); ++it)
    destNodes.insert(it->first);
  // Add destination node (if new)
  int dest=AddNode(destNodes);
  // Add transition
  myNodes[pos].AddTransition(input,DFATransition(dest,destEdge));
  return dest;
} // }}}
