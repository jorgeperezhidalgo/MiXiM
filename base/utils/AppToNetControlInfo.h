

#ifndef APPTONETCONTROLINFO_H
#define APPTONETCONTROLINFO_H

#include <omnetpp.h>

/**
 * Control Info to pass interface information from the application to the
 * NET layer Currently the only information necessary is the MAC
 * address of the next hop and the activation or not of the CSMA,
 * which has to be determined by ARP or some
 * similar mechanism
 *
 * @author Jorge Perez Hidalgo
 **/
class AppToNetControlInfo : public cObject
{
  protected:
    /** @brief MAC address of the receiving node*/
    int destAddr;
    // CSMA Active
    bool csmaActive;


  public:
    /** @brief Default constructor*/
    AppToNetControlInfo(const int addr) : destAddr(addr) {};

    //Constructor with BE
    AppToNetControlInfo(const int addr, const bool csma)
    {
    	destAddr = addr;
       	csmaActive = csma;
    };

    /** @brief Destructor*/
    virtual ~AppToNetControlInfo() {};

    /** @brief Getter method */
    virtual const int getDestAddr() {
    	return destAddr;
    };

    // Getter method
    virtual const int getCsmaActive() {
    	return csmaActive;
    };

    /** @brief Setter method */
    virtual void setDestAddr(const int addr){
    	destAddr = addr;
    };
    // Setter method
    virtual void setCsmaActive(const int csma) {
    	csmaActive = csma;
    };

};


#endif
