#include <rcp/nfa.hpp>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <rcp/common.hpp>
using namespace std;

NFATransition::NFATransition(int dest, const string &input, const string &output) // {{{
: myDest(dest),
  myInput(input),
  myOutput(output)
{
} // }}}
NFATransition::~NFATransition() // {{{
{
} // }}}
int NFATransition::GetDest() const // {{{
{
  return myDest;
} // }}}
const string &NFATransition::GetInput() const // {{{
{
  return myInput;
} // }}}
const string &NFATransition::GetOutput() const // {{{
{
  return myOutput;
} // }}}

NFANode::NFANode() // {{{
: myFinal(false)
{
  myTransitions.clear();
} // }}}
NFANode::~NFANode() // {{{
{
} // }}}
bool NFANode::Final() const // {{{
{
  return myFinal;
} // }}}
void NFANode::SetFinal(bool final) // {{{
{
  myFinal=final;
} // }}}
void NFANode::AddTransition(const NFATransition &trans) // {{{
{
  vector<NFATransition>::iterator it=myTransitions.begin();
  while (it!=myTransitions.end() && it->GetInput()<trans.GetInput())
    ++it;
  if (it==myTransitions.end())
    myTransitions.push_back(trans);
  else
    myTransitions.insert(it,trans);
} // }}}
int NFANode::CountTransitions() const // {{{
{
  return myTransitions.size();
} // }}}
const NFATransition &NFANode::GetTransition(int id) const // {{{
{
  return myTransitions[id];
} // }}}

NFA::NFA(const RE &exp) // {{{
{
  AddNode();
  int endid=Create(exp,0);
  myNodes[endid].SetFinal(true);
} // }}}
NFA::~NFA() // {{{
{
} // }}}
int NFA::AddNode() // {{{
{
  myNodes.push_back(NFANode());
  return myNodes.size()-1;
} // }}}
const NFANode &NFA::GetNode(int i) const // {{{
{
  return myNodes[i];
} // }}}
bool NFA::Deterministic() const // {{{
{
  for (vector<NFANode>::const_iterator it=myNodes.begin(); it!=myNodes.end(); ++it)
  {
    for (int edge=0; edge<=it->CountTransitions()-1; ++edge)
      if (is_prefix(it->GetTransition(edge).GetInput(),it->GetTransition(edge+1).GetInput()))
        return false;
  }
  return true;
} // }}}
bool NFA::Accept(const string &s) const // {{{
{
  set<int> empty;
  return Accept(s,0,empty);
} // }}}
bool NFA::Accept(const string &s, int pos, set<int> marked) const // {{{
{
  if (marked.find(pos) != marked.end())
    return false;
  marked.insert(pos);
  set<int> empty;

  if (s.size()==0 && myNodes[pos].Final())
    return true;

  for (int i=0; i<myNodes[pos].CountTransitions(); ++i)
    if (myNodes[pos].GetTransition(i).GetInput().size()==0)
    {
      if (Accept(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
                 myNodes[pos].GetTransition(i).GetDest(),
                 marked))
        return true;
    }
    else
      if (is_prefix(myNodes[pos].GetTransition(i).GetInput(),s) &&
          Accept(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
                 myNodes[pos].GetTransition(i).GetDest(),
                 empty))
        return true;
  return false;
} // }}}
string NFA::Compress(const string &s) const // {{{
{
  string result="";
  set<int> empty;
  if (Compress(s,0,result,empty))
  { reverse(result.begin(),result.end()); // reverse string
    return result;
  }
  else
    return "ERROR: NO MATCH";
} // }}}
bool NFA::Compress(const string &s, int pos, string &dest, set<int> marked) const // {{{
{
  if (marked.find(pos) != marked.end())
    return false;
  marked.insert(pos);

  if (s.size()==0 && myNodes[pos].Final())
  { dest="";
    return true;
  }

  for (int i=0; i<myNodes[pos].CountTransitions(); ++i)
    if (myNodes[pos].GetTransition(i).GetInput().size()==0)
    { if (Compress(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
          myNodes[pos].GetTransition(i).GetDest(),
          dest,
          marked))
      { dest+=myNodes[pos].GetTransition(i).GetOutput();
        return true;
      }
    }
    else
    { if (is_prefix(myNodes[pos].GetTransition(i).GetInput(),s) &&
          Compress(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
          myNodes[pos].GetTransition(i).GetDest(),
          dest,
          set<int>()))
      { dest+=myNodes[pos].GetTransition(i).GetOutput();
        return true;
      }
    }
  return false;
} // }}}
void NFA::Closure(std::map<int,std::pair<std::string,int> > &nodes) const // {{{
{
  std::map<int,std::pair<std::string,int> > newNodes;
  std::map<int,std::pair<std::string,int> > tmpNodes=nodes;
  // Continue while new nodes are found
  while (tmpNodes.size()>0)
  { // Iterate though the found nodes
    for (std::map<int,std::pair<std::string,int> >::iterator node=tmpNodes.begin(); node!=tmpNodes.end(); ++node)
    { // Consider each edge from that node
      for (int edge=0; edge<myNodes[node->first].CountTransitions(); ++edge)
      { // If epsilon-edge and destination is new, add it to the new nodes
        if (myNodes[node->first].GetTransition(edge).GetInput()=="" &&
            nodes.find(myNodes[node->first].GetTransition(edge).GetDest())==nodes.end())
        {
          newNodes[myNodes[node->first].GetTransition(edge).GetDest()].first=node->second.first + myNodes[node->first].GetTransition(edge).GetOutput();
          newNodes[myNodes[node->first].GetTransition(edge).GetDest()].second=node->second.second;
          nodes[myNodes[node->first].GetTransition(edge).GetDest()]=newNodes[myNodes[node->first].GetTransition(edge).GetDest()];
        }
      }
    }
    // Mark the new nodes to be checked for epsilon edges
    tmpNodes=newNodes;
    // Clear newNodes for next iteration
    newNodes.clear();
  }
  return;
} // }}}
string NFA::ToString() // {{{
{
  stringstream result;
  result << "digraph NFA\n{\n node [shape=circle]\n graph [rankdir=\"LR\"]\n start [shape=point]\n end [shape=point]\n start -> 0";
  for (int i=0; i<myNodes.size(); ++i)
  {
    if (myNodes[i].Final())
      result << "\n " << i << "->end";
    for (int j=0; j<myNodes[i].CountTransitions(); ++j)
    {
      result << "\n " << i << "->" << myNodes[i].GetTransition(j).GetDest()
             << " [label=\"" << myNodes[i].GetTransition(j).GetInput() << "/" << myNodes[i].GetTransition(j).GetOutput() << "\"]";
        ;
    }
  }
  result << "\n}";
  return result.str();
} // }}}
int NFA::Create(const RE &exp, int start) // {{{
{
  if (typeid(exp)==typeid(RE_One))
      return start;
  else if (typeid(exp)==typeid(RE_Char))
  { int nodeid=AddNode();
    string input="";
    input+=((RE_Char*)&exp)->GetChar();
    NFATransition edge(nodeid,input,"");
    myNodes[start].AddTransition(edge);
    return nodeid;
  }
  else if (typeid(exp)==typeid(RE_Seq))
  { int end1=Create(((RE_Seq*)&exp)->GetFront(),start);
    return Create(((RE_Seq*)&exp)->GetBack(),end1);
  }
  else if (typeid(exp)==typeid(RE_Sum))
  { int start1=AddNode();
    int start2=AddNode();
    int end1=Create(((RE_Sum*)&exp)->GetLeft(),start1);
    int end2=Create(((RE_Sum*)&exp)->GetRight(),start2);
    int end=AddNode();
    // Add Edges
    myNodes[start].AddTransition(NFATransition(start1,"","0"));
    myNodes[start].AddTransition(NFATransition(start2,"","1"));
    myNodes[end1].AddTransition(NFATransition(end,"",""));
    myNodes[end2].AddTransition(NFATransition(end,"",""));
    return end;
  }
  else if (typeid(exp)==typeid(RE_Star))
  { int start1=AddNode();
    int end1=Create(((RE_Star*)&exp)->GetSub(),start1);
    int end=AddNode();
    // Add Edges
    myNodes[start].AddTransition(NFATransition(start1,"","1"));
    myNodes[start].AddTransition(NFATransition(end,"","0"));
    myNodes[end1].AddTransition(NFATransition(start,"",""));
    return end;
  }
  else if (typeid(exp)==typeid(RE))
  { int nodeid=AddNode();
    return nodeid;
  }
  else  // No RE type recognised
  { cerr << "NFA::Create Error: Unknown RE constructor: " << exp.ToString() << endl;
    int nodeid=AddNode();
    return nodeid;
  }
} // }}}
