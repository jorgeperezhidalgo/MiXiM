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

#include "AnchorAppLayer.h"
#include "NetwControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>


Define_Module(AnchorAppLayer);

void AnchorAppLayer::initialize(int stage)
{
	BaseLayer::initialize(stage);

	if(stage == 0) {
		world = FindModule<BaseWorldUtility*>::findGlobalModule();

		arp = FindModule<BaseArp*>::findSubModule(findHost());
		myNetwAddr = arp->myNetwAddr(this);

        cc = FindModule<BaseConnectionManager *>::findGlobalModule();
        if( cc == 0 ) error("Could not find connectionmanager module");

		packetLength = par("packetLength");
		packetTime = par("packetTime");
		phaseRepetitionNumber = par("phaseRepetitionNumber");
		syncPacketsPerSyncPhase = par("syncPacketsPerSyncPhase");
		if (syncPacketsPerSyncPhase <= 0) error("You must introduce a positive number of periods.");
		syncPhaseNumber = 1;
		syncPacketsPerSyncPhaseCounter = 1;

		destination = par("destination");
		syncPacketTime = par("syncPacketTime");
		scheduledSlot = 0;
		phase2VIPPercentage = par("phase2VIPPercentage");

		syncInSlot = par("syncInSlot");
	   	//syncFirstMaxRandomTime = par("syncFirstMaxRandomTime");
		// Cambio temporal
		syncFirstMaxRandomTime = par("syncRestMaxRandomTimes");
	   	syncRestMaxRandomTimes = par("syncRestMaxRandomTimes");

		fullPhaseTime = getParentModule()->getParentModule()->par("fullPhaseTime");
		timeComSinkPhase = getParentModule()->getParentModule()->par("timeComSinkPhase");

		nbPacketDropped = 0;

	} else if (stage == 1) { //Asign the type to 1 to be taken in account in slot distribution
		anchor = cc->findNic(getParentModule()->findSubmodule("nic"));
		anchor->moduleType = 1;
	} else if (stage == 4) {
		anchor = cc->findNic(getParentModule()->findSubmodule("nic"));
		timeSyncPhase = anchor->numTotalSlots * syncPacketTime ;
		timeVIPPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * phase2VIPPercentage;
		timeReportPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * (1 - phase2VIPPercentage);

		delayTimer = new cMessage("sync-delay-timer", SEND_SYNC_TIMER_WITHOUT_CSMA);
		if (phaseRepetitionNumber != 0 && syncInSlot)
		{
			lastPhaseStart = simTime();
			scheduleAt(lastPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);

			scheduledSlot++;
			if (scheduledSlot >= anchor->numSlots)
			{
				scheduledSlot = 0;
				syncPacketsPerSyncPhaseCounter++;
				lastPhaseStart = lastPhaseStart + timeSyncPhase;
				if (syncPacketsPerSyncPhaseCounter > syncPacketsPerSyncPhase)
				{
					syncPacketsPerSyncPhaseCounter = 1;
					switch (syncPhaseNumber)
					{
					case 1:
						syncPhaseNumber++;
						lastPhaseStart = lastPhaseStart + timeReportPhase + timeVIPPhase;
						break;
					case 2:
						syncPhaseNumber++;
						lastPhaseStart = lastPhaseStart + timeComSinkPhase;
						break;
					case 3:
						syncPhaseNumber = 1;
						lastPhaseStart = lastPhaseStart + timeComSinkPhase;
						phaseRepetitionNumber--;
						break;
					default:
						syncPhaseNumber = 1;
					}
				}
			}
		}
		else if (phaseRepetitionNumber != 0)
		{
			scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
		}
		else
		{
//			// Try for the routing
//			if (getParentModule()->getIndex() == 21)
//			{
//				ApplPkt *pkt = new ApplPkt("Report with CSMA", REPORT_WITH_CSMA);
//				//int netwAddr = getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic");
//				int netwAddr = getParentModule()->getParentModule()->getSubmodule("anchor",0)->findSubmodule("nic");
//				pkt->setBitLength(packetLength);
////				pkt->setControlInfo(new AppToNetControlInfo(netwAddr, true));
//				pkt->setDestAddr(netwAddr);
//				pkt->setSrcAddr(getParentModule()->findSubmodule("nic"));
//				sendDown(pkt);
//			}
		}
	}
}

AnchorAppLayer::~AnchorAppLayer() {
	cancelAndDelete(delayTimer);
}


void AnchorAppLayer::finish()
{
	recordScalar("dropped", nbPacketDropped);

}

void AnchorAppLayer::handleSelfMsg(cMessage *msg)
{
	switch( msg->getKind() )
	{
	case SEND_SYNC_TIMER_WITHOUT_CSMA:
		assert(msg == delayTimer);

		sendBroadcast();

		if (phaseRepetitionNumber != 0 && syncInSlot)
		{
			scheduleAt(lastPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);

			scheduledSlot++;
			if (scheduledSlot >= anchor->numSlots)
			{
				scheduledSlot = 0;
				syncPacketsPerSyncPhaseCounter++;
				lastPhaseStart = lastPhaseStart + timeSyncPhase;
				if (syncPacketsPerSyncPhaseCounter > syncPacketsPerSyncPhase)
				{
					syncPacketsPerSyncPhaseCounter = 1;
					switch (syncPhaseNumber)
					{
					case 1:
						syncPhaseNumber++;
						lastPhaseStart = lastPhaseStart + timeReportPhase + timeVIPPhase;
						break;
					case 2:
						syncPhaseNumber++;
						lastPhaseStart = lastPhaseStart + timeComSinkPhase;
						break;
					case 3:
						syncPhaseNumber = 1;
						lastPhaseStart = lastPhaseStart + timeComSinkPhase;
						phaseRepetitionNumber--;
						break;
					default:
						syncPhaseNumber = 1;
					}
				}
			}
			EV <<"Time for next Sync Packet "<< lastPhaseStart<<endl;
		}
		else
		{

		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}


void AnchorAppLayer::handleLowerMsg(cMessage *msg)
{
//	Packet p(packetLength, 1, 0);
//	world->publishBBItem(catPacket, &p, -1);
	//get control info attached by base class decapsMsg method
	assert(dynamic_cast<NetwControlInfo*>(msg->getControlInfo()));
	NetwControlInfo* cInfo = static_cast<NetwControlInfo*>(msg->getControlInfo());
	EV << "La RSSI en Appl es: " << cInfo->getRSSI() << endl;
	EV << "La BER en Appl es: " << cInfo->getBitErrorRate() << endl;

	// Temporal aquí habrá que añadir todos los tipos de paquetes que tratar
	switch( msg->getKind() )
    {
    case AnchorAppLayer::REPORT_WITHOUT_CSMA:
    case AnchorAppLayer::REPORT_WITH_CSMA:
    	if ((static_cast<ApplPkt*>(msg))->getDestAddr() == getParentModule()->findSubmodule("nic")) {
    		getParentModule()->bubble("I've received the message");
    		delete msg;
    		msg = 0;
    	} else {
    		sendDown(msg);
    	}
    	break;
    default:
    	delete msg;
    	msg = 0;
    }
}


void AnchorAppLayer::handleLowerControl(cMessage *msg)
{
	if (!syncInSlot) {
		if(msg->getKind() == BaseMacLayer::PACKET_DROPPED) {
			nbPacketDropped++;
			nextSyncSend = uniform(0, syncRestMaxRandomTimes, 0);
			EV << "El envio del mensaje de sync ha fallado. Enviando otra vez mensaje " << syncPacketsPerSyncPhaseCounter << " de " << syncPacketsPerSyncPhase << " en " << nextSyncSend <<"s." << endl;
			// Temporal, this is ok only for first synctime, if want to use regularly we have to change it to it
			if ((simTime() + nextSyncSend) <= (timeSyncPhase * syncPacketsPerSyncPhase)) {
				scheduleAt(simTime() + nextSyncSend, delayTimer);
			}
		} else if (msg->getKind() == BaseMacLayer::SYNC_SENT) {
			syncPacketsPerSyncPhaseCounter++;
			EV << "El mensaje de sync se ha enviado correctamente.";
			if (syncPacketsPerSyncPhaseCounter <= syncPacketsPerSyncPhase) {
				nextSyncSend = uniform(0, syncRestMaxRandomTimes, 0);
				EV << "Enviando " << syncPacketsPerSyncPhaseCounter << " de " << syncPacketsPerSyncPhase << " en " << nextSyncSend <<"s.";
				// Temporal, this is ok only for first synctime, if want to use regularly we have to change it to it
				if ((simTime() + nextSyncSend) <= (timeSyncPhase * syncPacketsPerSyncPhase)) {
					scheduleAt(simTime() + nextSyncSend, delayTimer);
				}
			}
			EV << endl;
		}
	}
	delete msg;
	msg = 0;
}

void AnchorAppLayer::sendBroadcast()
{
	//It doesn't matter if we have slotted version or not, CSMA must be disabled, we control random time and retransmision in app layer
	ApplPkt *pkt = new ApplPkt("SYNC_MESSAGE_WITHOUT_CSMA", SYNC_MESSAGE_WITHOUT_CSMA);
	pkt->setBitLength(packetLength);

	pkt->setSrcAddr(myNetwAddr);
	pkt->setDestAddr(destination);

	pkt->setSequenceId(syncPacketsPerSyncPhaseCounter);



//	Packet p(packetLength, 0, 1);
//	world->publishBBItem(catPacket, &p, -1);

	sendDown(pkt);
}

