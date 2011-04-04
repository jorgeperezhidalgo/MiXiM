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
#include "NetwToMacControlInfo.h"
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

//		Packet p(1);
//		catPacket = world->getCategory(&p);
	} else if (stage == 1) {
		anchor = cc->findNic(getParentModule()->findSubmodule("nic"));
		anchor->moduleType = 1;
	} else if (stage == 4) {
		anchor = cc->findNic(getParentModule()->findSubmodule("nic"));
		timeSyncPhase = anchor->numTotalSlots * syncPacketTime ;
		timeVIPPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * phase2VIPPercentage;
		timeReportPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * (1 - phase2VIPPercentage);
		EV << "T Report: " << timeReportPhase << endl;
		EV << "T VIP: " << timeVIPPhase << endl;

		delayTimer = new cMessage("delay-timer", SEND_SYNC_TIMER);
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
			EV <<"Time for next Sync Packet "<< lastPhaseStart<<endl;
		}
		else
		{
			scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
		}
	}
}

int AnchorAppLayer::hasSlot(int *slots, int *slotCounter, int numSlots, int anchor, int numAnchors)
{
	int presenceInSlots = 0;
	for (int i = 0; i < numSlots; i++)
	{
		for (int j = 0; j < *(slotCounter + i); j++)
		{
			if (*(slots + (i * numAnchors) + j) == anchor)
				presenceInSlots++;
		}
	}
	return presenceInSlots;
}

void AnchorAppLayer::orderQueue(int *queue, int *numerTimesQueue, int queueCounter)
{
    int tempQueue[(queueCounter+1)]; memset(tempQueue, 0, sizeof(int)*(queueCounter+1));
    int tempNumerTimesQueue[(queueCounter+1)]; memset(tempNumerTimesQueue, 0, sizeof(int)*(queueCounter+1));
    int tempQueueCounter = 0;
    int posMax = 0;
    int posMin = 0;

    for (int i = 0; i < queueCounter; i++)
    {
    	tempQueue[i] = queue[i];
    	tempNumerTimesQueue[i] = numerTimesQueue[i];
    	if (numerTimesQueue[i] > numerTimesQueue[posMax])
    		posMax = i;
    }
    for (tempQueueCounter = 0; tempQueueCounter <= queueCounter; tempQueueCounter++)
    {
    	posMin = posMax;
    	for (int i = 0; i < queueCounter; i++)
    	{
    		if (tempNumerTimesQueue[i] <= tempNumerTimesQueue[posMin])
    		{
    			posMin = i;
    		}
    	}
    	queue[tempQueueCounter] = tempQueue[posMin];
    	numerTimesQueue[tempQueueCounter] = tempNumerTimesQueue[posMin];
    	tempNumerTimesQueue[posMin] = tempNumerTimesQueue[posMax] + 1;
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
	case SEND_SYNC_TIMER:
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

	delete msg;
	msg = 0;
}


void AnchorAppLayer::handleLowerControl(cMessage *msg)
{
	if (!syncInSlot) {
		if(msg->getKind() == BaseMacLayer::PACKET_DROPPED) {
			nbPacketDropped++;
			nextSyncSend = uniform(0, syncRestMaxRandomTimes, 0);
			EV << "El envio del mensaje de sync ha fallado. Enviando otra vez mensaje " << syncPacketsPerSyncPhaseCounter << " de " << syncPacketsPerSyncPhase << " en " << nextSyncSend <<"s." << endl;
			scheduleAt(simTime() + nextSyncSend, delayTimer);
		} else if (msg->getKind() == BaseMacLayer::SYNC_SENT) {
			syncPacketsPerSyncPhaseCounter++;
			EV << "El mensaje de sync se ha enviado correctamente.";
			if (syncPacketsPerSyncPhaseCounter <= syncPacketsPerSyncPhase) {
				nextSyncSend = uniform(0, syncRestMaxRandomTimes, 0);
				EV << "Enviando " << syncPacketsPerSyncPhaseCounter << " de " << syncPacketsPerSyncPhase << " en " << nextSyncSend <<"s.";
				scheduleAt(simTime() + nextSyncSend, delayTimer);
			}
			EV << endl;
		}
	}
	delete msg;
	msg = 0;
}

void AnchorAppLayer::sendBroadcast()
{
	SyncPkt *pkt = new SyncPkt("SYNC_MESSAGE", SYNC_MESSAGE);
	pkt->setBitLength(packetLength);

//	pkt->setSrcAddr(myNetwAddr);
//	pkt->setDestAddr(destination);

	//It doesn't matter if we have slotted version or not, CSMA must be disabled, we control random time and retransmision in app layer

	pkt->setControlInfo(new NetwToMacControlInfo(destination,false));

	pkt->setSequenceId(syncPacketsPerSyncPhaseCounter);



//	Packet p(packetLength, 0, 1);
//	world->publishBBItem(catPacket, &p, -1);

	sendDown(pkt);
}

