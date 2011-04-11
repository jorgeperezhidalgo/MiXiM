//
// Generated file, do not edit! Created by opp_msgc 4.1 from messages/SyncPkt.msg.
//

#ifndef _SYNCPKT_M_H_
#define _SYNCPKT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0401
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/SyncPkt.msg</tt> by opp_msgc.
 * <pre>
 * packet SyncPkt
 * {
 * 	int sequenceId = 0;
 * 	int srcAddr = 0;
 *     int8 status = 0;
 * 	int16 posX = 0;
 * 	int16 posY = 0;
 * 	int16 posZ = 0;
 * 	int32 timestamp = 0;
 * }
 * </pre>
 */
class SyncPkt : public ::cPacket
{
  protected:
    int sequenceId_var;
    int srcAddr_var;
    int8 status_var;
    int16 posX_var;
    int16 posY_var;
    int16 posZ_var;
    int32 timestamp_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const SyncPkt&);

  public:
    SyncPkt(const char *name=NULL, int kind=0);
    SyncPkt(const SyncPkt& other);
    virtual ~SyncPkt();
    SyncPkt& operator=(const SyncPkt& other);
    virtual SyncPkt *dup() const {return new SyncPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getSequenceId() const;
    virtual void setSequenceId(int sequenceId_var);
    virtual int getSrcAddr() const;
    virtual void setSrcAddr(int srcAddr_var);
    virtual int8 getStatus() const;
    virtual void setStatus(int8 status_var);
    virtual int16 getPosX() const;
    virtual void setPosX(int16 posX_var);
    virtual int16 getPosY() const;
    virtual void setPosY(int16 posY_var);
    virtual int16 getPosZ() const;
    virtual void setPosZ(int16 posZ_var);
    virtual int32 getTimestamp() const;
    virtual void setTimestamp(int32 timestamp_var);
};

inline void doPacking(cCommBuffer *b, SyncPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, SyncPkt& obj) {obj.parsimUnpack(b);}


#endif // _SYNCPKT_M_H_