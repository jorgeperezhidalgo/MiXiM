#include "AppToNetControlInfo.h"
#include "AnchorNetLayer.h"
#include "NetwControlInfo.h"
#include "AddressingInterface.h"
#include "NetwPkt_m.h"
#include <NetwToMacControlInfo.h>
#include <MacToNetwControlInfo.h>
#include <cassert>

Define_Module(AnchorNetLayer);

void AnchorNetLayer::initialize(int stage) {
	BaseNetwLayer::initialize(stage);
	if (stage == 0) {
        cc = FindModule<BaseConnectionManager *>::findGlobalModule();
        if( cc == 0 ) error("Could not find connectionmanager module");
	}
}
void AnchorNetLayer::finish() {
}

/**
 * Redefine this function if you want to process messages from lower
 * layers before they are forwarded to upper layers
 *
 *
 * If you want to forward the message to upper layers please use
 * @ref sendUp which will take care of decapsulation and thelike
 **/
void AnchorNetLayer::handleLowerMsg(cMessage* msg)
{
    NetwPkt *m = static_cast<NetwPkt *>(msg);
    EV << " handling packet from " << m->getSrcAddr() << endl;
    sendUp(decapsMsg(m));
}

/**
 * Redefine this function if you want to process control messages
 * from lower layers.
 **/
void AnchorNetLayer::handleLowerControl(cMessage* msg)
{
    // When we send a control message from Mac to Appl, we send the original message (in case we need to retransmit it)
	// so we have to decapsulate in Net also and change kind and name, to the ones we got from MAC
	const char *name = msg->getName();
	short kind = msg->getKind();
	cMessage *m = (static_cast<NetwPkt*>(msg))->decapsulate();
	m->setName(name);
	m->setKind(kind);
    sendControlUp(m);
    delete msg;
    msg = 0;
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 *
 * To forward the message to lower layers after processing it please
 * use @ref sendDown. It will take care of anything needed
 **/
void AnchorNetLayer::handleUpperMsg(cMessage* msg)
{
	assert(dynamic_cast<cPacket*>(msg));
    sendDown(encapsMsg(static_cast<cPacket*>(msg)));
}

/**
 * Redefine this function if you want to process control messages
 * from upper layers.
 **/
void AnchorNetLayer::handleUpperControl(cMessage* msg)
{
	sendControlDown(msg);
}


/**
 * Decapsulates the packet from the received Network packet
 **/
cMessage* AnchorNetLayer::decapsMsg(NetwPkt *msg)
{
    cMessage *m = msg->decapsulate();
	//get control info attached by base class decapsMsg method
	//and set its rssi and ber
	assert(dynamic_cast<MacToNetwControlInfo*>(msg->getControlInfo()));
	MacToNetwControlInfo* cInfo = static_cast<MacToNetwControlInfo*>(msg->getControlInfo());

	m->setControlInfo(new NetwControlInfo(msg->getSrcAddr(), cInfo->getBitErrorRate(), cInfo->getRSSI()));
    // delete the netw packet
    delete msg;
    return m;
}


/**
 * Encapsulates the received TransPkt into a NetwPkt and set all needed
 * header fields.
 **/
NetwPkt* AnchorNetLayer::encapsMsg(cPacket *appPkt) {
    int macAddr;
    int netwAddr;
    int origen;
    int destino;
	// Routing matrix for 25 elements initialization
	int routing_matrix_25[26][26] = { {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
		{1,1,2,3,3,6,6,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
		{2,2,2,3,4,2,2,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25},
		{3,3,3,3,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3},
		{6,6,6,6,6,5,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6},
		{2,2,2,2,2,5,6,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
		{8,8,8,8,8,8,8,7,8,8,11,11,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
		{25,25,25,25,25,25,25,7,8,25,7,7,12,25,25,12,12,12,25,25,12,12,12,25,25,25},
		{25,25,25,25,25,25,25,25,25,9,25,25,25,13,14,25,25,-1,13,14,25,25,25,13,14,25},
		{11,11,11,11,11,11,11,11,11,11,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11},
		{7,7,7,7,7,7,7,7,7,7,10,11,7,7,7,7,7,7,7,7,7,7,7,7,7,7},
		{8,8,8,8,8,8,8,8,8,8,8,8,12,8,8,16,16,17,8,8,16,17,17,8,8,8},
		{9,9,9,9,9,9,9,9,9,9,9,9,9,13,9,9,9,9,18,9,9,9,9,18,9,9},
		{9,9,9,9,9,9,9,9,9,9,9,9,9,9,14,9,9,9,9,19,9,9,9,9,19,9},
		{16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,15,16,16,16,16,16,16,16,16,16,16},
		{12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,15,16,12,12,12,20,12,12,12,12,12},
		{12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,17,12,12,12,21,22,12,12,12},
		{13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,18,13,13,13,13,23,13,13},
		{14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,19,14,14,14,14,24,14},
		{16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,20,16,16,16,16,16},
		{17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,21,17,17,17,17},
		{17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,22,17,17,17},
		{18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,23,18,18},
		{19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,24,19},
		{3,3,3,3,3,3,3,8,8,9,8,8,8,9,9,8,8,8,9,9,8,8,8,9,9,25} };


    EV <<"in encaps...\n";

    NetwPkt *pkt = new NetwPkt(appPkt->getName(), appPkt->getKind());
    pkt->setBitLength(headerLength);

    netwAddr = (static_cast<ApplPkt*>(appPkt))->getDestAddr();
    EV<<"NET received a message from upper layer, name is " << appPkt->getName() <<", dest mac addr="<< netwAddr <<endl;

    pkt->setSrcAddr(myNetwAddr);
    pkt->setDestAddr(netwAddr);
    EV << " netw "<< myNetwAddr << " sending packet" <<endl;
    if(netwAddr == L3BROADCAST) {
        EV << "sendDown: nHop=L3BROADCAST -> message has to be broadcasted"
           << " -> set destMac=L2BROADCAST\n";
        macAddr = L2BROADCAST;
    }
    else{
        EV <<"sendDown: get the MAC address\n";
		host = cc->findNic(simulation.getModule(myNetwAddr)->getParentModule()->findSubmodule("nic"));
		if (host->moduleType == 3)
			origen = 25;
		else
			origen = simulation.getModule(myNetwAddr)->getParentModule()->getIndex();

		host = cc->findNic(simulation.getModule(netwAddr)->getParentModule()->findSubmodule("nic"));
		if (host->moduleType == 3)
			destino = 25;
		else
			destino = simulation.getModule(netwAddr)->getParentModule()->getIndex();

		if (host->moduleType == 2) {
			macAddr = netwAddr;
		} else {
			if (routing_matrix_25[origen][destino] == 25)
				macAddr = arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic"));
			else
				macAddr = arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("anchor",routing_matrix_25[origen][destino])->findSubmodule("nic"));
		}
    }

    if ((static_cast<ApplPkt*>(appPkt))->getCSMA()) {
    	pkt->setControlInfo(new NetwToMacControlInfo(macAddr, true));
    } else {
    	pkt->setControlInfo(new NetwToMacControlInfo(macAddr, false));
    }


    //encapsulate the application packet
    pkt->encapsulate(appPkt);
    EV <<" pkt encapsulated\n";
    return pkt;
}
