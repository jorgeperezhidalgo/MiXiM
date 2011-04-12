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

#include "ComputerAppLayer.h"
#include "NetwToMacControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>


Define_Module(ComputerAppLayer);

void ComputerAppLayer::initialize(int stage)
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

		fullPhaseTime = getParentModule()->getParentModule()->par("fullPhaseTime");
		timeComSinkPhase = getParentModule()->getParentModule()->par("timeComSinkPhase");

		nbPacketDropped = 0;

//		Packet p(1);
//		catPacket = world->getCategory(&p);
//	} else if (stage == 3 && syncInSlot) {
	} else if (stage == 3) {
		BaseConnectionManager::NicEntries& anchorList = cc->getAnchorsList();
		numAnchors = anchorList.size();
		EV << "Numero de AN: " << numAnchors << endl;
		int j = 0;
		double distance;
		NicEntry* anchors[numAnchors];
		int matrix[numAnchors][numAnchors]; memset(matrix, 0, sizeof(int)*numAnchors*numAnchors);
		int slotCounter[numAnchors];  memset(slotCounter, 0, sizeof(int)*numAnchors);
		int slots[numAnchors][numAnchors]; memset(slots, 0, sizeof(int)*numAnchors*numAnchors);
		int queue[numAnchors]; memset(queue, 0, sizeof(int)*numAnchors);
		int numerTimesQueue[numAnchors]; memset(numerTimesQueue, 0, sizeof(int)*numAnchors);
		int queueCounter = 0;
		int hasSlots;
		numSlots = 0;
		bool lookRow;
		bool compatible;
//        EV << "   0  1  2  3  4  5  6  7  8  9  10 11 " << endl;
		EV << "Coverage " << cc->getMaxInterferenceDistance() << endl;
		for(BaseConnectionManager::NicEntries::iterator i = anchorList.begin(); i != anchorList.end(); ++i)
		{
			anchors[j] = i->second;
			anchors[j]->numSlots = 0;
			j++;
		}
		for (j = 0; j <= numAnchors-1; j++)
		{

			//EV << j << " Anchor " << anchors[j]->nicId << " type " << anchors[j]->moduleType;
			//EV << "PosNic despues (" << anchors[j]->pos.getX() << ", " << anchors[j]->pos.getY() << ")" << endl;
//			if (j<10)
//				EV << j << "  ";
//			else
//				EV << j << " ";

			lookRow = false;
			queueCounter = 0;
			if (hasSlot(*slots, slotCounter, numSlots, j, numAnchors) == 0)
			{
//				EV << "Añadiendo nuevo Slot " << numSlots << " con el anchor " << j << endl;
				numSlots++;
				slotCounter[numSlots-1]++;
				slots[numSlots-1][slotCounter[numSlots-1]-1] = j;
				lookRow = true;
			}
			for (int k = 0; k <= numAnchors-1; k++)
			{
				if (matrix[j][k] == 0)
				{
					distance = sqrt(pow(anchors[k]->pos.getX() - anchors[j]->pos.getX(),2) + pow(anchors[k]->pos.getY() - anchors[j]->pos.getY(),2));
					matrix[j][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
					matrix[k][j] = matrix[j][k];
				}
//        		EV << matrix[j][k] << "  ";
				if ((matrix[j][k] >= 3) && lookRow)
				{
					compatible = true;
					for (int l = 0; l < slotCounter[numSlots-1]; l++)
					{
						if (matrix[slots[numSlots-1][l]][k] == 0)
						{
							distance = sqrt(pow(anchors[k]->pos.getX() - anchors[slots[numSlots-1][l]]->pos.getX(),2) + pow(anchors[k]->pos.getY() - anchors[slots[numSlots-1][l]]->pos.getY(),2));
							matrix[slots[numSlots-1][l]][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
							matrix[k][slots[numSlots-1][l]] = matrix[slots[numSlots-1][l]][k];
						}
						if (matrix[slots[numSlots-1][l]][k] <= 2)
							compatible = false;
					}
					if (compatible)
					{
						hasSlots = hasSlot(*slots, slotCounter, numSlots, k, numAnchors);
						if (hasSlots == 0)
						{
//        					EV << "Añadiendo nuevo anchor " << k << " al slot " << numSlots-1 << endl;
							slotCounter[numSlots-1]++;
							slots[numSlots-1][slotCounter[numSlots-1]-1] = k;
						}
						else
						{
							queue[queueCounter] = k;
							numerTimesQueue[queueCounter] = hasSlots;
							queueCounter++;
						}
					}
				}
			}
			orderQueue(queue, numerTimesQueue, queueCounter);
			for (int k = 0; k < queueCounter; k++)
			{
				compatible = true;
				for (int l = 0; l < slotCounter[numSlots-1]; l++)
				{
					if (matrix[slots[numSlots-1][l]][queue[k]] == 0)
					{
						distance = sqrt(pow(anchors[queue[k]]->pos.getX() - anchors[slots[numSlots-1][l]]->pos.getX(),2) + pow(anchors[queue[k]]->pos.getY() - anchors[slots[numSlots-1][l]]->pos.getY(),2));
						matrix[slots[numSlots-1][l]][queue[k]] = int((distance / cc->getMaxInterferenceDistance()) + 1);
						matrix[queue[k]][slots[numSlots-1][l]] = matrix[slots[numSlots-1][l]][queue[k]];
					}
					if (matrix[slots[numSlots-1][l]][queue[k]] <= 2)
						compatible = false;
				}
				if (compatible)
				{
//       				EV << "Reutilizando anchor " << queue[k] << " en el slot " << numSlots-1 << endl;
					slotCounter[numSlots-1]++;
					slots[numSlots-1][slotCounter[numSlots-1]-1] = queue[k];
				}
			}
//        	EV << endl;
		}

		for (j = 0; j < numSlots; j++)
		{
			EV << "Slot " << j << ": ";
			for (int k = 0; k < slotCounter[j]; k++)
			{
				anchors[slots[j][k]]->transmisionSlot[anchors[slots[j][k]]->numSlots] = j;
				anchors[slots[j][k]]->numSlots = anchors[slots[j][k]]->numSlots +1;
				anchors[slots[j][k]]->numTotalSlots = numSlots;
				EV << "(" << anchors[slots[j][k]]->numSlots << ")";
				EV << slots[j][k] << ",";
			}
			EV << endl;
		}
		j= 0;
		for(BaseConnectionManager::NicEntries::iterator i = anchorList.begin(); i != anchorList.end(); ++i)
		{
			EV << "Nic (" << j << ")" << i->second->nicId << " transmits in Slot: ";
			for (int k = 0; k < i->second->numSlots; k++)
			{
				EV << i->second->transmisionSlot[k] << ", ";
			}
			EV << endl;
			j++;
		}
		timeSyncPhase = numSlots * syncPacketTime ;
		timeVIPPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * phase2VIPPercentage;
		timeReportPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * syncPacketsPerSyncPhase * timeSyncPhase)) * (1 - phase2VIPPercentage);
		EV << "T Report: " << timeReportPhase << endl;
		EV << "T VIP: " << timeVIPPhase << endl;
		EV << "Number of Slots: " << numSlots << endl;
	}
}

int ComputerAppLayer::hasSlot(int *slots, int *slotCounter, int numSlots, int anchor, int numAnchors)
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

void ComputerAppLayer::orderQueue(int *queue, int *numerTimesQueue, int queueCounter)
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

ComputerAppLayer::~ComputerAppLayer() {
}


void ComputerAppLayer::finish()
{
	recordScalar("dropped", nbPacketDropped);
	recordScalar("numSlots", numSlots);
}

void ComputerAppLayer::handleSelfMsg(cMessage *msg)
{
	switch( msg->getKind() )
	{
	case SEND_MESSAGE_TIMER:
		assert(msg == delayTimer);

		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}


void ComputerAppLayer::handleLowerMsg(cMessage *msg)
{
//	Packet p(packetLength, 1, 0);
//	world->publishBBItem(catPacket, &p, -1);

	delete msg;
	msg = 0;
}


void ComputerAppLayer::handleLowerControl(cMessage *msg)
{
	if (!syncInSlot) {
		if(msg->getKind() == BaseMacLayer::PACKET_DROPPED) {
			nbPacketDropped++;
		}
	}
	delete msg;
	msg = 0;
}

void ComputerAppLayer::sendBroadcast()
{
	SyncPkt *pkt = new SyncPkt("SYNC_MESSAGE", MESSAGE);
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

