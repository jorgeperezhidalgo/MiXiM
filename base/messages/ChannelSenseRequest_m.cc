//
// Generated file, do not edit! Created by opp_msgc 4.1 from messages/ChannelSenseRequest.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ChannelSenseRequest_m.h"

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




EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("SenseMode");
    if (!e) enums.getInstance()->add(e = new cEnum("SenseMode"));
    e->insert(UNTIL_IDLE, "UNTIL_IDLE");
    e->insert(UNTIL_BUSY, "UNTIL_BUSY");
    e->insert(UNTIL_TIMEOUT, "UNTIL_TIMEOUT");
);

Register_Class(ChannelSenseRequest);

ChannelSenseRequest::ChannelSenseRequest(const char *name, int kind) : cPacket(name,kind)
{
    this->senseMode_var = 0;
    this->senseTimeout_var = 0;
}

ChannelSenseRequest::ChannelSenseRequest(const ChannelSenseRequest& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

ChannelSenseRequest::~ChannelSenseRequest()
{
}

ChannelSenseRequest& ChannelSenseRequest::operator=(const ChannelSenseRequest& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->senseMode_var = other.senseMode_var;
    this->senseTimeout_var = other.senseTimeout_var;
    this->result_var = other.result_var;
    return *this;
}

void ChannelSenseRequest::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->senseMode_var);
    doPacking(b,this->senseTimeout_var);
    doPacking(b,this->result_var);
}

void ChannelSenseRequest::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->senseMode_var);
    doUnpacking(b,this->senseTimeout_var);
    doUnpacking(b,this->result_var);
}

int ChannelSenseRequest::getSenseMode() const
{
    return senseMode_var;
}

void ChannelSenseRequest::setSenseMode(int senseMode_var)
{
    this->senseMode_var = senseMode_var;
}

simtime_t ChannelSenseRequest::getSenseTimeout() const
{
    return senseTimeout_var;
}

void ChannelSenseRequest::setSenseTimeout(simtime_t senseTimeout_var)
{
    this->senseTimeout_var = senseTimeout_var;
}

ChannelState& ChannelSenseRequest::getResult()
{
    return result_var;
}

void ChannelSenseRequest::setResult(const ChannelState& result_var)
{
    this->result_var = result_var;
}

class ChannelSenseRequestDescriptor : public cClassDescriptor
{
  public:
    ChannelSenseRequestDescriptor();
    virtual ~ChannelSenseRequestDescriptor();

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

Register_ClassDescriptor(ChannelSenseRequestDescriptor);

ChannelSenseRequestDescriptor::ChannelSenseRequestDescriptor() : cClassDescriptor("ChannelSenseRequest", "cPacket")
{
}

ChannelSenseRequestDescriptor::~ChannelSenseRequestDescriptor()
{
}

bool ChannelSenseRequestDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ChannelSenseRequest *>(obj)!=NULL;
}

const char *ChannelSenseRequestDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ChannelSenseRequestDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int ChannelSenseRequestDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *ChannelSenseRequestDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "senseMode",
        "senseTimeout",
        "result",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int ChannelSenseRequestDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senseMode")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "senseTimeout")==0) return base+1;
    if (fieldName[0]=='r' && strcmp(fieldName, "result")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ChannelSenseRequestDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "simtime_t",
        "ChannelState",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *ChannelSenseRequestDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0:
            if (!strcmp(propertyname,"enum")) return "SenseMode";
            return NULL;
        default: return NULL;
    }
}

int ChannelSenseRequestDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ChannelSenseRequest *pp = (ChannelSenseRequest *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ChannelSenseRequestDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ChannelSenseRequest *pp = (ChannelSenseRequest *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getSenseMode());
        case 1: return double2string(pp->getSenseTimeout());
        case 2: {std::stringstream out; out << pp->getResult(); return out.str();}
        default: return "";
    }
}

bool ChannelSenseRequestDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ChannelSenseRequest *pp = (ChannelSenseRequest *)object; (void)pp;
    switch (field) {
        case 0: pp->setSenseMode(string2long(value)); return true;
        case 1: pp->setSenseTimeout(string2double(value)); return true;
        default: return false;
    }
}

const char *ChannelSenseRequestDescriptor::getFieldStructName(void *object, int field) const
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
        "ChannelState",
    };
    return (field>=0 && field<3) ? fieldStructNames[field] : NULL;
}

void *ChannelSenseRequestDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ChannelSenseRequest *pp = (ChannelSenseRequest *)object; (void)pp;
    switch (field) {
        case 2: return (void *)(&pp->getResult()); break;
        default: return NULL;
    }
}


