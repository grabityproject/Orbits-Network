class CNode
{
  //.....
  void PushInventory(const CInv& inv)
  {
    CRITICAL_BLOCK(cs_inventory)
      if(!setInventoryKnown.count(inv))
        vInventoryToSend.push_back(inv);
  }
//......
}
inline void RelayInventory(const CInv& inv)
{
  //Put on lists to offer to the other nodes
  CRITICAL_BLOCK(cs_vNodes)
    foreach(CNode* pnode, vNodes)
      pnode->PushInventory(inv);
}
template<typename T>
void RelayMessage(const CInv& inv, const T& a)
{
  CDataStream ss(SER_NETWORK);
  ss.reserve(10000);
  ss << a;
  RelayMessage(inv, ss);
}
template<>
inline void RelayMessage<>(const CInv& inv, const CDataStream& ss)
{
  CRITICAL_BLOCK(cs_mapRelay)
  {
    //Expire old relay messages
    while(!vRelayExpiration.empty() && vRelayExpiration.front().first < GetTime())
    {
      mapRelay.erase(vRelayExpiration.front().second);
      vRelayExpiration.pop_fron();
    }
    //Save original serialized message so newer versions are preserved
    mapRelay[inv] = ss;
    vRelayExpiration.push_back(make_pair(GetTime() + 15 * 60, inv));
  }
  RelayInventory(inv)
}
