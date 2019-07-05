bool AddAddress(CAddrDB& addrdb, const CAddress& addr)
{
  if(!addr.IsRoutable())
    return false;
  if(addr.ip == addrLocalHost.ip)
    return false;
  CRITICAL_BLOCK(cs_mapAddresses)
  {
    map<vector<unsigned char>, CAddress>::iterator it =
      mapAddresses.find(addr.GetKey());
    if(it == mapAddresses.end())
    {
      //New address
      mapAddresses.insert(make_pair(addr.GetKey(), addr));
      addrdb.WriteAddress(addr);
      return true;
    }
    else
    {
      CAddress& addrFound = (*it).second;
      if((addrFound.nServices | addr.nServies) != addrFound.nServices)
      {
        //Services have been added
        addrFound.nServices |= addr.nServices;
        addrdb.WriteAddress(addrFound);
        return true;
      }
    }
  }
  return false;
}
