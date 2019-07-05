void AddOrphanTx(const CDataStream& vMsg)
{
  CTransaction tx;
  CDataStream(vMsg) >> tx;
  uint256 hash = tx.GetHash();
  if(mapOrphanTransactions.count(hash))
    return;
  CDataStream* pvMsg = mapOrphanTransactions[hash] = new CDataStream(vMsg);
  foreach(const CTxIn& txin, tx.vin)
    mapOrphanTransactionsByPrev.insert(make_pair(txin.prevout.hash, pvMsg));
}
void EraseOrhpanTx(uint256 hash)
{
  if(!mapOrphanTransactions.count(hash))
    return;
  const CDataStream* pvMsg = mapOrphanTransactions[hash];
  CTransaction tx;
  CDataStream(*pvMsg) >> tx;
  foreach(const CTxIn& txin, tx.vin)
  {
    for(multimap<uint256, CDataStream*>::iterator mi =
      mapOrphanTransactionsByPrev.lower_bound(txin.prevout.hash);
        mi != mapOrphanTransactionsByPrev.upper_bound(txin.prevout.hash);)
        {
          if((*mi).second == pvMsg)
            mapOrphanTransactionsByPrev.erase(mi++);
          else
            mi++;
        }
  }
  delete pvMsg;
  mapOrphanTransactions.erase(hash);
}
