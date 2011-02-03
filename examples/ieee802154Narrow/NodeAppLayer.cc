//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "NodeAppLayer.h"
#include "NetwToMacControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>


Define_Module(NodeAppLayer);

void NodeAppLayer::initialize(int stage)
{
	BaseLayer::initialize(stage);

	if(stage == 0) {
		world = FindModule<BaseWorldUtility*>::findGlobalModule();
//		delayTimer = new cMessage("delay-timer", SEND_SYNC_TIMER);

		arp = FindModule<BaseArp*>::findSubModule(findHost());
		myNetwAddr = arp->myNetwAddr(this);

        cc = FindModule<BaseConnectionManager *>::findGlobalModule();
        if( cc == 0 ) error("Could not find connectionmanager module");

		packetLength = par("packetLength");
		packetTime = par("packetTime");
		destination = par("destination");

		nbPacketDropped = 0;

	} else if (stage == 1) {
		(cc->findNic(getParentModule()->findSubmodule("nic")))->moduleType = 2;
	} else {

	}
}

NodeAppLayer::~NodeAppLayer() {
//	cancelAndDelete(delayTimer);
}


void NodeAppLayer::finish()
{
	recordScalar("dropped", nbPacketDropped);
}

void NodeAppLayer::handleSelfMsg(cMessage *msg)
{
//	switch( msg->getKind() )
//	{
//	case SEND_SYNC_TIMER:
//		assert(msg == delayTimer);
//
//
//		sendBroadcast();
//
//		remainingBurst--;
//
//		if(remainingBurst == 0) {
//			//remainingBurst = burstSize;
//			EV << "Se acabo" <<endl;
//			scheduleAt(simTime() + (dblrand()*1.4+0.3)*packetTime * burstSize / pppt, msg);
//		} else {
//			scheduleAt(simTime() + packetTime * 2, msg);
//		}
//
//		break;
//	default:
//		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
//		delete msg;
//	}
}


void NodeAppLayer::handleLowerMsg(cMessage *msg)
{
//	Packet p(packetLength, 1, 0);
//	world->publishBBItem(catPacket, &p, -1);

	switch( msg->getKind() )
	{
	case SYNC_MESSAGE:
		EV << "Lowermessage " << msg->getName() << " received in Appl Layer in Node " << this->getId() << endl;
		break;
	default:
		EV << "Unkown lowermessage! -> delete, kind: "<< msg->getKind() << endl;
	}

	delete msg;
	msg = 0;
}


void NodeAppLayer::handleLowerControl(cMessage *msg)
{
	if(msg->getKind() == BaseMacLayer::PACKET_DROPPED) {
		nbPacketDropped++;
	}
	delete msg;
	msg = 0;
}

void NodeAppLayer::sendBroadcast()
{
//	NetwPkt *pkt = new NetwPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);
//	pkt->setBitLength(packetLength);
//
//	pkt->setSrcAddr(myNetwAddr);
//	pkt->setDestAddr(destination);
//
//	pkt->setControlInfo(new NetwToMacControlInfo(destination));
//
//	Packet p(packetLength, 0, 1);
//	world->publishBBItem(catPacket, &p, -1);
//
//	sendDown(pkt);
}

