//
// Generated file, do not edit! Created by opp_msgc 4.1 from messages/ApplPkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ApplPkt_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(ApplPkt);

ApplPkt::ApplPkt(const char *name, int kind) : cPacket(name,kind)
{
    this->destAddr_var = -1;
    this->srcAddr_var = -1;
    this->sequenceId_var = 0;
    this->priority_var = 0;
    this->status_var = 0;
    this->posX_var = 0;
    this->posY_var = 0;
    this->posZ_var = 0;
    this->timestamp_var = 0;
    this->realDestAddr_var = -1;
    this->realSrcAddr_var = -1;
    this->retransmisionCounterBO_var = 0;
    this->retransmisionCounterACK_var = 0;
    this->CSMA_var = true;
    this->askForRequest_var = false;
    this->requestPacket_var = false;
    this->numberOfBroadcasts_var = 0;
}

ApplPkt::ApplPkt(const ApplPkt& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

ApplPkt::~ApplPkt()
{
}

ApplPkt& ApplPkt::operator=(const ApplPkt& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->destAddr_var = other.destAddr_var;
    this->srcAddr_var = other.srcAddr_var;
    this->sequenceId_var = other.sequenceId_var;
    this->priority_var = other.priority_var;
    this->status_var = other.status_var;
    this->posX_var = other.posX_var;
    this->posY_var = other.posY_var;
    this->posZ_var = other.posZ_var;
    this->timestamp_var = other.timestamp_var;
    this->realDestAddr_var = other.realDestAddr_var;
    this->realSrcAddr_var = other.realSrcAddr_var;
    this->retransmisionCounterBO_var = other.retransmisionCounterBO_var;
    this->retransmisionCounterACK_var = other.retransmisionCounterACK_var;
    this->CSMA_var = other.CSMA_var;
    this->askForRequest_var = other.askForRequest_var;
    this->requestPacket_var = other.requestPacket_var;
    this->numberOfBroadcasts_var = other.numberOfBroadcasts_var;
    return *this;
}

void ApplPkt::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->destAddr_var);
    doPacking(b,this->srcAddr_var);
    doPacking(b,this->sequenceId_var);
    doPacking(b,this->priority_var);
    doPacking(b,this->status_var);
    doPacking(b,this->posX_var);
    doPacking(b,this->posY_var);
    doPacking(b,this->posZ_var);
    doPacking(b,this->timestamp_var);
    doPacking(b,this->realDestAddr_var);
    doPacking(b,this->realSrcAddr_var);
    doPacking(b,this->retransmisionCounterBO_var);
    doPacking(b,this->retransmisionCounterACK_var);
    doPacking(b,this->CSMA_var);
    doPacking(b,this->askForRequest_var);
    doPacking(b,this->requestPacket_var);
    doPacking(b,this->numberOfBroadcasts_var);
}

void ApplPkt::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->destAddr_var);
    doUnpacking(b,this->srcAddr_var);
    doUnpacking(b,this->sequenceId_var);
    doUnpacking(b,this->priority_var);
    doUnpacking(b,this->status_var);
    doUnpacking(b,this->posX_var);
    doUnpacking(b,this->posY_var);
    doUnpacking(b,this->posZ_var);
    doUnpacking(b,this->timestamp_var);
    doUnpacking(b,this->realDestAddr_var);
    doUnpacking(b,this->realSrcAddr_var);
    doUnpacking(b,this->retransmisionCounterBO_var);
    doUnpacking(b,this->retransmisionCounterACK_var);
    doUnpacking(b,this->CSMA_var);
    doUnpacking(b,this->askForRequest_var);
    doUnpacking(b,this->requestPacket_var);
    doUnpacking(b,this->numberOfBroadcasts_var);
}

int ApplPkt::getDestAddr() const
{
    return destAddr_var;
}

void ApplPkt::setDestAddr(int destAddr_var)
{
    this->destAddr_var = destAddr_var;
}

int ApplPkt::getSrcAddr() const
{
    return srcAddr_var;
}

void ApplPkt::setSrcAddr(int srcAddr_var)
{
    this->srcAddr_var = srcAddr_var;
}

int ApplPkt::getSequenceId() const
{
    return sequenceId_var;
}

void ApplPkt::setSequenceId(int sequenceId_var)
{
    this->sequenceId_var = sequenceId_var;
}

int ApplPkt::getPriority() const
{
    return priority_var;
}

void ApplPkt::setPriority(int priority_var)
{
    this->priority_var = priority_var;
}

int8 ApplPkt::getStatus() const
{
    return status_var;
}

void ApplPkt::setStatus(int8 status_var)
{
    this->status_var = status_var;
}

int16 ApplPkt::getPosX() const
{
    return posX_var;
}

void ApplPkt::setPosX(int16 posX_var)
{
    this->posX_var = posX_var;
}

int16 ApplPkt::getPosY() const
{
    return posY_var;
}

void ApplPkt::setPosY(int16 posY_var)
{
    this->posY_var = posY_var;
}

int16 ApplPkt::getPosZ() const
{
    return posZ_var;
}

void ApplPkt::setPosZ(int16 posZ_var)
{
    this->posZ_var = posZ_var;
}

int32 ApplPkt::getTimestamp() const
{
    return timestamp_var;
}

void ApplPkt::setTimestamp(int32 timestamp_var)
{
    this->timestamp_var = timestamp_var;
}

int ApplPkt::getRealDestAddr() const
{
    return realDestAddr_var;
}

void ApplPkt::setRealDestAddr(int realDestAddr_var)
{
    this->realDestAddr_var = realDestAddr_var;
}

int ApplPkt::getRealSrcAddr() const
{
    return realSrcAddr_var;
}

void ApplPkt::setRealSrcAddr(int realSrcAddr_var)
{
    this->realSrcAddr_var = realSrcAddr_var;
}

int ApplPkt::getRetransmisionCounterBO() const
{
    return retransmisionCounterBO_var;
}

void ApplPkt::setRetransmisionCounterBO(int retransmisionCounterBO_var)
{
    this->retransmisionCounterBO_var = retransmisionCounterBO_var;
}

int ApplPkt::getRetransmisionCounterACK() const
{
    return retransmisionCounterACK_var;
}

void ApplPkt::setRetransmisionCounterACK(int retransmisionCounterACK_var)
{
    this->retransmisionCounterACK_var = retransmisionCounterACK_var;
}

bool ApplPkt::getCSMA() const
{
    return CSMA_var;
}

void ApplPkt::setCSMA(bool CSMA_var)
{
    this->CSMA_var = CSMA_var;
}

bool ApplPkt::getAskForRequest() const
{
    return askForRequest_var;
}

void ApplPkt::setAskForRequest(bool askForRequest_var)
{
    this->askForRequest_var = askForRequest_var;
}

bool ApplPkt::getRequestPacket() const
{
    return requestPacket_var;
}

void ApplPkt::setRequestPacket(bool requestPacket_var)
{
    this->requestPacket_var = requestPacket_var;
}

int ApplPkt::getNumberOfBroadcasts() const
{
    return numberOfBroadcasts_var;
}

void ApplPkt::setNumberOfBroadcasts(int numberOfBroadcasts_var)
{
    this->numberOfBroadcasts_var = numberOfBroadcasts_var;
}

class ApplPktDescriptor : public cClassDescriptor
{
  public:
    ApplPktDescriptor();
    virtual ~ApplPktDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ApplPktDescriptor);

ApplPktDescriptor::ApplPktDescriptor() : cClassDescriptor("ApplPkt", "cPacket")
{
}

ApplPktDescriptor::~ApplPktDescriptor()
{
}

bool ApplPktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ApplPkt *>(obj)!=NULL;
}

const char *ApplPktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ApplPktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 17+basedesc->getFieldCount(object) : 17;
}

unsigned int ApplPktDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<17) ? fieldTypeFlags[field] : 0;
}

const char *ApplPktDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "destAddr",
        "srcAddr",
        "sequenceId",
        "priority",
        "status",
        "posX",
        "posY",
        "posZ",
        "timestamp",
        "realDestAddr",
        "realSrcAddr",
        "retransmisionCounterBO",
        "retransmisionCounterACK",
        "CSMA",
        "askForRequest",
        "requestPacket",
        "numberOfBroadcasts",
    };
    return (field>=0 && field<17) ? fieldNames[field] : NULL;
}

int ApplPktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddr")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddr")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequenceId")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "status")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "posX")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "posY")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "posZ")==0) return base+7;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+8;
    if (fieldName[0]=='r' && strcmp(fieldName, "realDestAddr")==0) return base+9;
    if (fieldName[0]=='r' && strcmp(fieldName, "realSrcAddr")==0) return base+10;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmisionCounterBO")==0) return base+11;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmisionCounterACK")==0) return base+12;
    if (fieldName[0]=='C' && strcmp(fieldName, "CSMA")==0) return base+13;
    if (fieldName[0]=='a' && strcmp(fieldName, "askForRequest")==0) return base+14;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestPacket")==0) return base+15;
    if (fieldName[0]=='n' && strcmp(fieldName, "numberOfBroadcasts")==0) return base+16;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ApplPktDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
        "int8",
        "int16",
        "int16",
        "int16",
        "int32",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "bool",
        "bool",
        "int",
    };
    return (field>=0 && field<17) ? fieldTypeStrings[field] : NULL;
}

const char *ApplPktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ApplPktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ApplPktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getDestAddr());
        case 1: return long2string(pp->getSrcAddr());
        case 2: return long2string(pp->getSequenceId());
        case 3: return long2string(pp->getPriority());
        case 4: return long2string(pp->getStatus());
        case 5: return long2string(pp->getPosX());
        case 6: return long2string(pp->getPosY());
        case 7: return long2string(pp->getPosZ());
        case 8: return long2string(pp->getTimestamp());
        case 9: return long2string(pp->getRealDestAddr());
        case 10: return long2string(pp->getRealSrcAddr());
        case 11: return long2string(pp->getRetransmisionCounterBO());
        case 12: return long2string(pp->getRetransmisionCounterACK());
        case 13: return bool2string(pp->getCSMA());
        case 14: return bool2string(pp->getAskForRequest());
        case 15: return bool2string(pp->getRequestPacket());
        case 16: return long2string(pp->getNumberOfBroadcasts());
        default: return "";
    }
}

bool ApplPktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        case 0: pp->setDestAddr(string2long(value)); return true;
        case 1: pp->setSrcAddr(string2long(value)); return true;
        case 2: pp->setSequenceId(string2long(value)); return true;
        case 3: pp->setPriority(string2long(value)); return true;
        case 4: pp->setStatus(string2long(value)); return true;
        case 5: pp->setPosX(string2long(value)); return true;
        case 6: pp->setPosY(string2long(value)); return true;
        case 7: pp->setPosZ(string2long(value)); return true;
        case 8: pp->setTimestamp(string2long(value)); return true;
        case 9: pp->setRealDestAddr(string2long(value)); return true;
        case 10: pp->setRealSrcAddr(string2long(value)); return true;
        case 11: pp->setRetransmisionCounterBO(string2long(value)); return true;
        case 12: pp->setRetransmisionCounterACK(string2long(value)); return true;
        case 13: pp->setCSMA(string2bool(value)); return true;
        case 14: pp->setAskForRequest(string2bool(value)); return true;
        case 15: pp->setRequestPacket(string2bool(value)); return true;
        case 16: pp->setNumberOfBroadcasts(string2long(value)); return true;
        default: return false;
    }
}

const char *ApplPktDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<17) ? fieldStructNames[field] : NULL;
}

void *ApplPktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


