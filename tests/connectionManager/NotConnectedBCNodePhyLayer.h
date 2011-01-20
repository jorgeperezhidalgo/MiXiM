/* -*- mode:c++ -*- ********************************************************
 * file:        NotConnectedBCNodePhyLayer.h
 *
 * author:      Karl Wessel
 ***************************************************************************
 * part of:     mixim framework
 * description: physical layer which broadcasts packets and expects no
				answer
 ***************************************************************************/


#ifndef NOT_CONNECTED_BCNODE_PHY_LAYER_H
#define NOT_CONNECTED_BCNODE_PHY_LAYER_H

#include "CMPhyLayer.h"

class NotConnectedBCNodePhyLayer : public CMPhyLayer
{
public:
    //Module_Class_Members(NotConnectedBCNodePhyLayer, CMPhyLayer, 0);

	bool broadcastAnswered;

	virtual void initialize(int stage) {
		CMPhyLayer::initialize(stage);
		if(stage==0){
			broadcastAnswered = false;
			scheduleAt(simTime() + 1.0 + (double)myAddr() * 0.1, new cMessage(0,10));
		}
	}

	virtual void finish() {
		CMPhyLayer::finish();

		assertFalse("Broadcast should not be answered by any node.",
					broadcastAnswered);
	}
protected:
	virtual void handleLowerMsg(int srcAddr) {
		broadcastAnswered = true;
		ev << "Not Connected BC-Node " << myAddr() << ": got answer message from " << srcAddr << endl;
	}

	virtual void handleSelfMsg() {
		// we should send a broadcast packet ...
		ev << "Not Connected BC-Node " << myAddr() << ": Sending broadcast packet!" << endl;
		sendDown(-1);
	}
};

#endif

