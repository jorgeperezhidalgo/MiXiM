//
// Generated file, do not edit! Created by opp_msgc 4.1 from messages/ApplPkt.msg.
//

#ifndef _APPLPKT_M_H_
#define _APPLPKT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0401
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/ApplPkt.msg</tt> by opp_msgc.
 * <pre>
 * packet ApplPkt
 * {
 *     int destAddr = -1; 				
 *     int srcAddr = -1; 				
 *     int sequenceId = 0;				
 *     int priority = 0; 				
 *     int8 status = 0;
 * 	int16 posX = 0;
 * 	int16 posY = 0;
 * 	int16 posZ = 0;
 * 	int32 timestamp = 0;
 * 	
 * 	
 * 
 * 	int realDestAddr = -1;			
 * 	int realSrcAddr = -1;			
 * 	int retransmisionCounterBO = 0;	
 * 	int retransmisionCounterACK = 0;
 * 	bool CSMA = true;				
 * 	bool askForRequest = false;		
 * 	bool requestPacket = false;		
 * 	int numberOfBroadcasts = 0;		
 * }
 * </pre>
 */
class ApplPkt : public ::cPacket
{
  protected:
    int destAddr_var;
    int srcAddr_var;
    int sequenceId_var;
    int priority_var;
    int8 status_var;
    int16 posX_var;
    int16 posY_var;
    int16 posZ_var;
    int32 timestamp_var;
    int realDestAddr_var;
    int realSrcAddr_var;
    int retransmisionCounterBO_var;
    int retransmisionCounterACK_var;
    bool CSMA_var;
    bool askForRequest_var;
    bool requestPacket_var;
    int numberOfBroadcasts_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ApplPkt&);

  public:
    ApplPkt(const char *name=NULL, int kind=0);
    ApplPkt(const ApplPkt& other);
    virtual ~ApplPkt();
    ApplPkt& operator=(const ApplPkt& other);
    virtual ApplPkt *dup() const {return new ApplPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getDestAddr() const;
    virtual void setDestAddr(int destAddr_var);
    virtual int getSrcAddr() const;
    virtual void setSrcAddr(int srcAddr_var);
    virtual int getSequenceId() const;
    virtual void setSequenceId(int sequenceId_var);
    virtual int getPriority() const;
    virtual void setPriority(int priority_var);
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
    virtual int getRealDestAddr() const;
    virtual void setRealDestAddr(int realDestAddr_var);
    virtual int getRealSrcAddr() const;
    virtual void setRealSrcAddr(int realSrcAddr_var);
    virtual int getRetransmisionCounterBO() const;
    virtual void setRetransmisionCounterBO(int retransmisionCounterBO_var);
    virtual int getRetransmisionCounterACK() const;
    virtual void setRetransmisionCounterACK(int retransmisionCounterACK_var);
    virtual bool getCSMA() const;
    virtual void setCSMA(bool CSMA_var);
    virtual bool getAskForRequest() const;
    virtual void setAskForRequest(bool askForRequest_var);
    virtual bool getRequestPacket() const;
    virtual void setRequestPacket(bool requestPacket_var);
    virtual int getNumberOfBroadcasts() const;
    virtual void setNumberOfBroadcasts(int numberOfBroadcasts_var);
};

inline void doPacking(cCommBuffer *b, ApplPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ApplPkt& obj) {obj.parsimUnpack(b);}


#endif // _APPLPKT_M_H_
