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
void NFANode::RemoveTransition(int id) // {{{
{
  myTransitions.erase(myTransitions.begin()+id);
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
int NFA::CountNodes() // {{{
{ return myNodes.size();
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
BitCode NFA::Compress(const string &s) const // {{{
{
  BitCode result;
  set<int> empty;
  if (Compress(s,0,result,empty))
  { result=result.Reverse(); // reverse bits
    return result;
  }
  else
    throw (string)"Error: No Match";
} // }}}
bool NFA::Compress(const string &s, int pos, BitCode &dest, set<int> marked) const // {{{
{
  if (marked.find(pos) != marked.end())
    return false;
  marked.insert(pos);

  if (s.size()==0 && myNodes[pos].Final())
    return true;

  for (int i=0; i<myNodes[pos].CountTransitions(); ++i)
    if (myNodes[pos].GetTransition(i).GetInput().size()==0)
    { if (Compress(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
          myNodes[pos].GetTransition(i).GetDest(),
          dest,
          marked))
      { string s=myNodes[pos].GetTransition(i).GetOutput();
        reverse(s.begin(),s.end());
        dest.Append(s);
        return true;
      }
    }
    else
    { if (is_prefix(myNodes[pos].GetTransition(i).GetInput(),s) &&
          Compress(s.substr(myNodes[pos].GetTransition(i).GetInput().size()),
          myNodes[pos].GetTransition(i).GetDest(),
          dest,
          set<int>()))
      { string s=myNodes[pos].GetTransition(i).GetOutput();
        reverse(s.begin(),s.end());
        dest.Append(s);
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
void NFA::Closure(std::map<int,BitCode> &nodes, const BCOrder &order) const // {{{
{
  map<int,BitCode> tmpNodes=nodes;
  // Continue while new nodes are found
  while (tmpNodes.size()>0)
  { // Consider the least node
    map<int,BitCode>::iterator node=tmpNodes.begin();
    // Consider each edge from that node
    for (int edge=0; edge<myNodes[node->first].CountTransitions(); ++edge)
    { 
      BitCode code=node->second; // Source code
      for (int bit=0; bit<GetNode(node->first).GetTransition(edge).GetOutput().size(); ++bit)
      { // Add extra bits
        code.PushBit(GetNode(node->first).GetTransition(edge).GetOutput()[bit]=='1');
      }
      // If epsilon-edge and creates better path, add it to the new nodes
      if (myNodes[node->first].GetTransition(edge).GetInput()=="" &&
          (nodes.find(myNodes[node->first].GetTransition(edge).GetDest())==nodes.end() ||
	   order.LEQ(code,nodes.find(myNodes[node->first].GetTransition(edge).GetDest())->second) ) )
      {
        tmpNodes[myNodes[node->first].GetTransition(edge).GetDest()]=code;
        nodes[myNodes[node->first].GetTransition(edge).GetDest()]=tmpNodes[myNodes[node->first].GetTransition(edge).GetDest()];
      }
    }
    // Clear the considered node from tmpNodes
    tmpNodes.erase(node);
  }
  return;
} // }}}
BitCode NFA::Thompson(const string &s, const BCOrder &order) const // {{{
{
  typedef map<int,BitCode> state;
  state cur_state;
  cur_state[0]=BitCode();
  // Perform epsilon-closure
  Closure(cur_state,order);
  for (int pos=0; pos<s.size() && !cur_state.empty(); ++pos)
  { // Find direct edges
    state next_state;
    for (state::const_iterator node=cur_state.begin(); node!=cur_state.end(); ++node)
    { for (int edge=0; edge<GetNode(node->first).CountTransitions(); ++edge)
      { if (GetNode(node->first).GetTransition(edge).GetInput().size()==1 &&
            GetNode(node->first).GetTransition(edge).GetInput()[0]==s[pos])
        { // Create bitcode
	  BitCode code=node->second; // Source code
	  for (int bit=0; bit<GetNode(node->first).GetTransition(edge).GetOutput().size(); ++bit)
          { // Add extra bits
            code.PushBit(GetNode(node->first).GetTransition(edge).GetOutput()[bit]=='1');
          }
	  // Set state if better path is found
	  if (next_state.find(GetNode(node->first).GetTransition(edge).GetDest())==next_state.end() ||
	      order.LEQ(code,next_state.find(GetNode(node->first).GetTransition(edge).GetDest())->second))
            next_state[GetNode(node->first).GetTransition(edge).GetDest()]=code;
        }
      }
    }
    // Perform epsilon-closure
    Closure(next_state,order);

    cur_state=next_state;
  }

  for (state::const_iterator node=cur_state.begin(); node!=cur_state.end(); ++node)
    if (GetNode(node->first).Final())
      return node->second;
  throw (string)"Error: No Match";
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
    myNodes[start].AddTransition(NFATransition(start1,"",BC_INL_STR));
    myNodes[start].AddTransition(NFATransition(start2,"",BC_INR_STR));
    myNodes[end1].AddTransition(NFATransition(end,"",""));
    myNodes[end2].AddTransition(NFATransition(end,"",""));
    return end;
  }
  else if (typeid(exp)==typeid(RE_Star))
  { int start1=AddNode();
    int end1=Create(((RE_Star*)&exp)->GetSub(),start1);
    int end=AddNode();
    // Add Edges
    myNodes[start].AddTransition(NFATransition(start1,"",BC_CONS_STR));
    myNodes[start].AddTransition(NFATransition(end,"",BC_NIL_STR));
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
void NFA::RemoveDeadStates() // {{{
{ map<int,int> newIndex;
  // Initially mark all nodes as dead (-1 for dead, 0 for alive)
  for (int i=0; i<myNodes.size(); ++i)
    newIndex[i]=-1;
  // Recursively mark all final nodes or nodes with an edge to a live node as live
  bool isDone=false;
  while (!isDone)
  { isDone = true;
    for (int i=0; i<myNodes.size(); ++i)
    { if (newIndex[i]<0)
      { if (myNodes[i].Final())
        { newIndex[i]=0;
          isDone=false;
        }
        else
        { for (int j=0; j<myNodes[i].CountTransitions(); ++j)
          { if (newIndex[myNodes[i].GetTransition(j).GetDest()]>=0)
            { newIndex[i]=0;
              isDone=false;
            }
          }
        }
      }
    }
  }
  // Always keep initial node
  newIndex[0]=0;
  int maxIndex=0;
  // Assign new indexes incrementally
  for (int i=1; i<myNodes.size(); ++i)
    if (newIndex[i]>=0)
      newIndex[i]=++maxIndex;
  // Make new NFA
  vector<NFANode> newNodes;
  for (int i=0; i<myNodes.size(); ++i)
  { if (newIndex[i]>=0)
    { // Create new node
      NFANode newNode;
      if (myNodes[i].Final())
        newNode.SetFinal(true);
      // Add edges to the new node
      for (int j=0; j<myNodes[i].CountTransitions(); ++j)
        if (newIndex[myNodes[i].GetTransition(j).GetDest()]>=0)
          newNode.AddTransition(
            NFATransition(newIndex[myNodes[i].GetTransition(j).GetDest()],
                          myNodes[i].GetTransition(j).GetInput(),
                          myNodes[i].GetTransition(j).GetOutput()));
      newNodes.push_back(newNode);
    }
  }
  // Use the updated NFA
  myNodes=newNodes;
} // }}}
void NFA::RemoveUnreachableStates() // {{{
{ map<int,int> newIndex;
  // Initially mark only the initial node as reachable (-1 for unreachable, 1 for reachable)
  newIndex[0]=0;
  for (int i=1; i<myNodes.size(); ++i)
    newIndex[i]=-1;
  // Recursively mark all final nodes or nodes with an edge to a live node as live
  bool isDone=false;
  while (!isDone)
  { isDone = true;
    for (int i=0; i<myNodes.size(); ++i)
    { if (newIndex[i]>=0)
      { for (int j=0; j<myNodes[i].CountTransitions(); ++j)
        { if (newIndex[myNodes[i].GetTransition(j).GetDest()]<0)
          { newIndex[myNodes[i].GetTransition(j).GetDest()]=0;
            isDone=false;
          }
        }
      }
    }
  }
  int maxIndex=0;
  // Assign new indexes incrementally
  for (int i=1; i<myNodes.size(); ++i)
    if (newIndex[i]>=0)
      newIndex[i]=++maxIndex;
  // Make new NFA
  vector<NFANode> newNodes;
  for (int i=0; i<myNodes.size(); ++i)
  { if (newIndex[i]>=0)
    { // Create new node
      NFANode newNode;
      if (myNodes[i].Final())
        newNode.SetFinal(true);
      // Add edges to the new node
      for (int j=0; j<myNodes[i].CountTransitions(); ++j)
        if (newIndex[myNodes[i].GetTransition(j).GetDest()]>=0)
          newNode.AddTransition(
            NFATransition(newIndex[myNodes[i].GetTransition(j).GetDest()],
                          myNodes[i].GetTransition(j).GetInput(),
                          myNodes[i].GetTransition(j).GetOutput()));
      newNodes.push_back(newNode);
    }
  }
  // Use the updated NFA
  myNodes=newNodes;
} // }}}
vector<NFATransition> NFA::CompactEdges(const NFATransition &edge) // {{{
{ vector<NFATransition> result;
  for (int j=0; j<myNodes[edge.GetDest()].CountTransitions(); ++j)
  { // Use all non-outputting continuations (recursively)
    if (myNodes[edge.GetDest()].GetTransition(j).GetOutput()=="")
    { vector<NFATransition> edges=CompactEdges(
        NFATransition(myNodes[edge.GetDest()].GetTransition(j).GetDest(),
                      edge.GetInput()+myNodes[edge.GetDest()].GetTransition(j).GetInput(),
                      edge.GetOutput()));
      result.insert(result.end(),edges.begin(),edges.end());
    }
  }
  // If edge cannot be continued, use original edge
  if (result.size()==0)
    result.push_back(edge);
  return result;
} // }}}
void NFA::MakeCompact() // {{{
{ vector<NFANode> newNodes;
  // Iterate through each transition from each node
  for (int i=0; i<myNodes.size(); ++i)
  { // Create new node
    NFANode newNode;
    if (myNodes[i].Final())
      newNode.SetFinal(true);
    for (int j=0; j<myNodes[i].CountTransitions(); ++j)
    { // Make all continuation for each edge
      vector<NFATransition> cEdges = CompactEdges(myNodes[i].GetTransition(j));
      for (vector<NFATransition>::const_iterator cEdge=cEdges.begin();
           cEdge!=cEdges.end(); ++cEdge)
        newNode.AddTransition(*cEdge);
    }
    newNodes.push_back(newNode);
  }
  // Use updated NFA
  myNodes = newNodes;
} // }}}
int find_group(vector<vector<int> > &groups, int node) // {{{
{ for (int i=0; i<groups.size(); ++i)
  for (int j=0; j<groups[i].size(); ++j)
    if (node==groups[i][j])
      return i;
  throw (string)"Error: Node not in any group";
} // }}}
void NFA::Reduce() // {{{
{ vector<vector<int> > groups;
  { vector<int> q1; // non-final nodes
    vector<int> q2; // final nodes
    for (int i=0; i<myNodes.size(); ++i)
    { if (myNodes[i].Final())
        q2.push_back(i);
      else
        q1.push_back(i);
    }
    groups.push_back(q1);
    groups.push_back(q2);
  }
  bool done=false;
  while(!done) // Continue until the groups are consistent
  { done = true;
    for (int group=0; group<groups.size(); ++group) // Check consistency of each group
    { vector<vector<int> > subgroups; // Create maximal consistent subgroups
      for (int node=0; node<groups[group].size(); ++node)
      { bool foundgroup=false;
        for (int subgroup=0; !foundgroup && subgroup<subgroups.size(); ++subgroup)
        { bool match=true;
          // Check that each transitions are in the group
          for (int tr_n=0; tr_n<myNodes[groups[group][node]].CountTransitions(); ++tr_n)
          { bool match_edge=false;
            for (int tr_g=0; !match_edge && tr_g<myNodes[subgroups[subgroup][0]].CountTransitions(); ++tr_g)
              if (myNodes[groups[group][node]].GetTransition(tr_n).GetInput()                   == myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetInput() &&
                  myNodes[groups[group][node]].GetTransition(tr_n).GetOutput()                  == myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetOutput() &&
                  find_group(groups,myNodes[groups[group][node]].GetTransition(tr_n).GetDest()) == find_group(groups,myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetDest()))
                match_edge=true;
            if (!match_edge)
              match=false;
          }
          // Check that each transitions are in the node
          for (int tr_g=0; tr_g<myNodes[subgroups[subgroup][0]].CountTransitions(); ++tr_g)
          { bool match_edge=false;
            for (int tr_n=0; !match_edge && tr_n<myNodes[groups[group][node]].CountTransitions(); ++tr_n)
              if (myNodes[groups[group][node]].GetTransition(tr_n).GetInput()                   == myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetInput() &&
                  myNodes[groups[group][node]].GetTransition(tr_n).GetOutput()                  == myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetOutput() &&
                  find_group(groups,myNodes[groups[group][node]].GetTransition(tr_n).GetDest()) == find_group(groups,myNodes[subgroups[subgroup][0]].GetTransition(tr_g).GetDest()))
                match_edge=true;
            if (!match_edge)
              match=false;
          }
          if (match) // add node to subgroup
          { foundgroup=true;
            subgroups[subgroup].push_back(groups[group][node]);
          }
        }
        if (!foundgroup)
        { vector<int> newsubgroup;
          newsubgroup.push_back(groups[group][node]);
          subgroups.push_back(newsubgroup);
        }
      }
      // Check if group was consistent
      if (subgroups.size()>1) // if not, replace group by subgroups
      { done=false;
        groups.erase(groups.begin()+group);
        for (int i=0; i<subgroups.size(); ++i)
          groups.push_back(subgroups[i]);
        --group;
      }
    }
  }
  // Move group with 0 to front
  int init_group_id = find_group(groups,0);
  vector<int> init_group=groups[init_group_id];
  groups.erase(groups.begin()+init_group_id);
  groups.insert(groups.begin(),init_group);
  // Merge groups
  vector<NFANode> newNodes;
  for (int group=0; group<groups.size(); ++group)
  { NFANode newNode;
    // Set final if group is final
    if (myNodes[groups[group][0]].Final())
      newNode.SetFinal(true);
    // Add (translated) edges
    for (int edge=0; edge<myNodes[groups[group][0]].CountTransitions(); ++edge)
      newNode.AddTransition(NFATransition(
        find_group(groups,myNodes[groups[group][0]].GetTransition(edge).GetDest()),
        myNodes[groups[group][0]].GetTransition(edge).GetInput(),
        myNodes[groups[group][0]].GetTransition(edge).GetOutput()));
    newNodes.push_back(newNode);
  }
  // Use reduced NFA
  myNodes=newNodes;
} // }}}
void NFA::Explode() // {{{
{ for (int node=0; node<myNodes.size(); ++node) // For each edge in each node
    for (int edge=0; edge<myNodes[node].CountTransitions(); ++edge)
    { if (myNodes[node].GetTransition(edge).GetInput().size()>1) // split transition
      { int dest = myNodes[node].GetTransition(edge).GetDest();
        string input = myNodes[node].GetTransition(edge).GetInput();
        string output = myNodes[node].GetTransition(edge).GetOutput();
        // Remove original edge
        myNodes[node].RemoveTransition(edge);
        --edge;
        // Replace with sequence of edges
        int e_source=node;
        for (int c=0; c<input.size(); ++c)
        { int e_dest=dest;
          string e_output="";
          if (c<input.size()-1)
          { myNodes.push_back(NFANode());
            e_dest=myNodes.size()-1;
          }
          if (c==0)
            e_output=output;
          string e_input=(string)""+input[c];
          myNodes[e_source].AddTransition(NFATransition(e_dest,e_input,e_output));
          e_source=e_dest;
        }
      }
    }
} // }}}
vector<string> NFA::FindPrefixes() // {{{
{ // For each node, find the longest common prefix from each path from the note to a final node
  vector<string> results;
  // Create initial (empty) prefixes
  for (int node=0; node<myNodes.size(); ++node)
    results.push_back("");
  // Loop while the prefixes increase
  bool updated=true;
  while (updated)
  { // No updates at the beginning of loop
    updated = false;
    // Update prefix for each node
    for (int node=0; node<myNodes.size(); ++node)
    { if (myNodes[node].Final())
        continue;
      vector<string> continuations;
      // Find possible continuations from node
      for (int edge=0; edge<myNodes[node].CountTransitions(); ++edge)
      {
        continuations.push_back(myNodes[node].GetTransition(edge).GetInput()
        + results[myNodes[node].GetTransition(edge).GetDest()]);
      }
      // Start check prefix from position of last prefix
      int pos = results[node].size();
      bool equal=true;
      while (equal)
      { // Only has prefix if it has continuations
        if (continuations.size()>0 && continuations[0].size()>pos)
        { // Use the first continuation as reference for prefix
          char nextch=continuations[0][pos];
          // Check that all other continuations has same prefix
          for (int i=1; i<continuations.size(); ++i)
          { if (continuations[i].size()<=pos || nextch!=continuations[i][pos])
              equal=false;
          }
          //
          if (equal) // We have found an update
          { updated=true;
            ++pos;
          }
        }
        else equal=false;
      }
      // Update prefix of node
      if (continuations.size()>0)
        results[node]=continuations[0].substr(0,pos);
    }
  }
  return results;
} // }}}
void NFA::MakePrefix() // {{{
{ // Find the prefix for each node 
  vector<string> prefixes=FindPrefixes();
  vector<NFANode> newNodes;
  // Create new initial node
  newNodes.push_back(NFANode());
  newNodes[0].AddTransition(NFATransition(1,prefixes[0],""));
  for (int node=0; node<myNodes.size(); ++node)
  { // Copy nodes from original transducer
    newNodes.push_back(NFANode());
    if (myNodes[node].Final())
      newNodes[node+1].SetFinal(true);
    for (int edge=0; edge<myNodes[node].CountTransitions(); ++edge)
    { // Add transformed edges
      string input = myNodes[node].GetTransition(edge).GetInput()
                   + prefixes[myNodes[node].GetTransition(edge).GetDest()];
      input=input.substr(prefixes[node].size()); // Remove prefix of source node
      newNodes[node+1].AddTransition(NFATransition(myNodes[node].GetTransition(edge).GetDest()+1,
                                                   input,
                                                   myNodes[node].GetTransition(edge).GetOutput()));
    }
  }
  // Use result
  myNodes=newNodes;
} // }}}
