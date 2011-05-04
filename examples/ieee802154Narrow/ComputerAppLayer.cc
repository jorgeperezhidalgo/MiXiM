#include "ComputerAppLayer.h"

Define_Module(ComputerAppLayer);

void ComputerAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if (stage == 1) {
		// Assign the type of host to 3 (computer)
		computer = cc->findNic(getParentModule()->findSubmodule("nic"));
		computer->moduleType = 3;
	// We have to wait till stage 3 to make this because in case the anchors are randomly situated, it's done in stage 2
	} else if (stage == 3) { // Slot calculation
		BaseConnectionManager::NicEntries& nicList = cc->getNicList();
		EV << "Number of AN: " << numberOfAnchors << endl;
		int j = 0;
		double distance;
		NicEntry* anchors[numberOfAnchors]; // Array to store pointers to all the anchors
		int matrix[numberOfAnchors][numberOfAnchors]; memset(matrix, 0, sizeof(int)*numberOfAnchors*numberOfAnchors); // Distance matrix between all Anchors 1, 2, 3, ... according to the maxinterferencedistance
		int slotCounter[numberOfAnchors];  memset(slotCounter, 0, sizeof(int)*numberOfAnchors); // Counts the number of elements in each slot
		int slots[numberOfAnchors][numberOfAnchors]; memset(slots, 0, sizeof(int)*numberOfAnchors*numberOfAnchors); // Stores the AN numbers in every slot and slot position
		int queue[numberOfAnchors]; memset(queue, 0, sizeof(int)*numberOfAnchors); // Variable to store the compatible Anchors for this slot but already have an slot, to analyze them at the end of the row
		int numerTimesQueue[numberOfAnchors]; memset(numerTimesQueue, 0, sizeof(int)*numberOfAnchors); // The number of times an element in this queue position already appeared, to order them and give more priority the ones that appeared the less
		int queueCounter = 0;
		int hasSlots;
		numTotalSlots = 0;
		bool lookRow;
		bool compatible;
//		EV << "   0  1  2  3  4  5  6  7  8  9  10 11 " << endl;
		EV << "Coverage " << cc->getMaxInterferenceDistance() << endl; // Maximal interference distance
		// First here we look for the Anchors and discard computer an MN, and put them in our array
		for(BaseConnectionManager::NicEntries::iterator i = nicList.begin(); i != nicList.end(); ++i) {
			if (i->second->moduleType == 1) {
				anchors[j] = i->second;
				anchors[j]->numSlots = 0;
				j++;
			}
		}
		// Cover the anchors array
		for (j = 0; j <= numberOfAnchors-1; j++) {

//		EV << j << " Anchor " << anchors[j]->nicId << " type " << anchors[j]->moduleType;
//		EV << "PosNic despues (" << anchors[j]->pos.getX() << ", " << anchors[j]->pos.getY() << ")" << endl;
//		if (j<10)
//			EV << j << "  ";
//		else
//			EV << j << " ";

			lookRow = false;
			queueCounter = 0;
			// If this anchor doesn't have an slot, we create a new one for it, if it's already assigned, we skip the row
			if (hasSlot(*slots, slotCounter, numTotalSlots, j, numberOfAnchors) == 0)
			{
//				EV << "Añadiendo nuevo Slot " << numTotalSlots << " con el anchor " << j << endl;
				numTotalSlots++; // Incremente the number of Slots
				slotCounter[numTotalSlots-1]++; // Increments the number of elements in this Slot
				slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = j; // Assign this slot and slot position the number of this AN (j variable)
				lookRow = true;
			}

			for (int k = 0; k <= numberOfAnchors-1; k++)
			{
				// This "if" takes out the 1, 2, 3 ... distance form the real value
				if (matrix[j][k] == 0)
				{
					distance = sqrt(pow(anchors[k]->pos.getX() - anchors[j]->pos.getX(),2) + pow(anchors[k]->pos.getY() - anchors[j]->pos.getY(),2));
					matrix[j][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
					matrix[k][j] = matrix[j][k];
				}
//        		EV << matrix[j][k] << "  ";
				// If distance >= 3 means we don't have hidden terminal problem
				// "lookrow" indicates that this element wasn't in any slot so we have to check his row
				if ((matrix[j][k] >= 3) && lookRow)
				{
					// This "for" checks if the possible new element in the slot is compatible with all the elements that are already in
					compatible = true;
					for (int l = 0; l < slotCounter[numTotalSlots-1]; l++)
					{
						if (matrix[slots[numTotalSlots-1][l]][k] == 0)
						{
							distance = sqrt(pow(anchors[k]->pos.getX() - anchors[slots[numTotalSlots-1][l]]->pos.getX(),2) + pow(anchors[k]->pos.getY() - anchors[slots[numTotalSlots-1][l]]->pos.getY(),2));
							matrix[slots[numTotalSlots-1][l]][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
							matrix[k][slots[numTotalSlots-1][l]] = matrix[slots[numTotalSlots-1][l]][k];
						}
						if (matrix[slots[numTotalSlots-1][l]][k] <= 2)
							compatible = false;
					}
					if (compatible) // If the new possible element is compatible with all the existing elements, goes inside the slot
					{
						hasSlots = hasSlot(*slots, slotCounter, numTotalSlots, k, numberOfAnchors);
						// If this new element hadn't already an slot its directly added to this slot
						if (hasSlots == 0)
						{
//        					EV << "Adding new anchor " << k << " to the slot " << numTotalSlots-1 << endl;
							slotCounter[numTotalSlots-1]++;
							slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = k;
						}
						else // If it had an slot, goes to a queue to check it later and give more opportunities to the rest of elements of the row
						{
							queue[queueCounter] = k;
							numerTimesQueue[queueCounter] = hasSlots;
							queueCounter++;
						}
					}
				}
			}
			orderQueue(queue, numerTimesQueue, queueCounter); // Orders the queue in ascendent order according to the number of assigned slots
			// This "for" checks the queue to assign extra slots to the nodes that are still compatibles
			for (int k = 0; k < queueCounter; k++)
			{
				// This "for" checks if the possible new element is compatible with the rest of the elements of the row
				compatible = true;
				for (int l = 0; l < slotCounter[numTotalSlots-1]; l++)
				{
					if (matrix[slots[numTotalSlots-1][l]][queue[k]] == 0)
					{
						distance = sqrt(pow(anchors[queue[k]]->pos.getX() - anchors[slots[numTotalSlots-1][l]]->pos.getX(),2) + pow(anchors[queue[k]]->pos.getY() - anchors[slots[numTotalSlots-1][l]]->pos.getY(),2));
						matrix[slots[numTotalSlots-1][l]][queue[k]] = int((distance / cc->getMaxInterferenceDistance()) + 1);
						matrix[queue[k]][slots[numTotalSlots-1][l]] = matrix[slots[numTotalSlots-1][l]][queue[k]];
					}
					if (matrix[slots[numTotalSlots-1][l]][queue[k]] <= 2)
						compatible = false;
				}
				if (compatible) // If its compatible, then we add it to the slot
				{
//       				EV << "Reusing anchor " << queue[k] << " in the slot " << numTotalSlots-1 << endl;
					slotCounter[numTotalSlots-1]++;
					slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = queue[k];
				}
			}
//        	EV << endl;
		}

		// This for prints the SLot configuration and assigns slot number values
		for (j = 0; j < numTotalSlots; j++)
		{
			EV << "Slot " << j << ": ";
			for (int k = 0; k < slotCounter[j]; k++)
			{
				anchors[slots[j][k]]->transmisionSlot[anchors[slots[j][k]]->numSlots] = j;
				anchors[slots[j][k]]->numSlots = anchors[slots[j][k]]->numSlots +1;
				anchors[slots[j][k]]->numTotalSlots = numTotalSlots;
				EV << "(" << anchors[slots[j][k]]->numSlots << ")";
				EV << slots[j][k] << ",";
			}
			EV << endl;
		}
		computer->numTotalSlots = numTotalSlots; // Asign in the computer NIC the total number of slots to have it accessible from other hosts

		// This for prints the Slots in which every Anchor transmits (sometimes they could have more than one)
		j= 0;
		for(BaseConnectionManager::NicEntries::iterator i = nicList.begin(); i != nicList.end(); ++i)
		{
			if (i->second->moduleType == 1) {
				EV << "Nic (" << j << ")" << i->second->nicId << " transmits in Slot: ";
				for (int k = 0; k < i->second->numSlots; k++)
				{
					EV << i->second->transmisionSlot[k] << ", ";
				}
				EV << endl;
				j++;
			}
		}
	} else if (stage == 4) {
		// Justs print the time on the screen to see them when we launch the simulation
		EV << "Number of Slots: " << numTotalSlots << endl;
		EV << "T Sync: " << timeSyncPhase << endl;
		EV << "T Report: " << timeReportPhase << endl;
		EV << "T VIP: " << timeVIPPhase << endl;
		EV << "T Com_Sink: " << timeComSinkPhase << endl;

		// Schedule the first queue processing at the start of the Com Sink 2
		checkQueue = new cMessage("transmit queue elements", CHECK_QUEUE);
		startTimeComSink2 = simTime() + timeSyncPhase + timeReportPhase + timeVIPPhase + timeSyncPhase + timeComSinkPhase + timeSyncPhase;
		scheduleAt(startTimeComSink2 , checkQueue);
		queueElementCounter = -1;
	}
}

ComputerAppLayer::~ComputerAppLayer() {
	cancelAndDelete(checkQueue);
}

void ComputerAppLayer::finish()
{
	recordScalar("dropped_AN", nbPacketDroppedReportAN);
	recordScalar("dropped_backoff_AN", nbPacketDroppedBackOffAN);
	recordScalar("numTotalSlots", numTotalSlots);
	recordScalar("dropped_no_time_in_Com_Sink", nbPacketDroppedNoTimeAN);
	recordScalar("packets_rcv_in_wrong_phase", numOfPcktsRcvdInWrongPhase);
}

void ComputerAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case CHECK_QUEUE:
		// If we didn't transmit any element of the queue is the moment to calculate all the random transmission times
		if (queueElementCounter == -1) {
			if (packetsQueue.length() > 0) { // Only if the Queue has elements we do calculate all the intermediate times
				stepTimeComSink2 = (timeComSinkPhase - guardTimeComSinkPhase) / packetsQueue.length();
				randomQueueTime = (simtime_t*)calloc(sizeof(simtime_t), packetsQueue.length());
				for (int i = 0; i < packetsQueue.length(); i++) {
					randomQueueTime[i] = startTimeComSink2 + (i * stepTimeComSink2) + uniform(0, stepTimeComSink2, 0);
				}
			}
			startTimeComSink2 = simTime() + fullPhaseTime; // Calculate now the start of the next Com Sink 2 phase
		} else {
			cMessage * msg = (cMessage *)packetsQueue.pop();
			EV << "Sending packet from the Queue" << endl;
			sendDown(msg);
		}
		// Calculates the next time this selfmessage will be scheduled, in this fullphase (still elements in queue) or in the next one
		if (packetsQueue.length() == 0) {
			queueElementCounter = -1;
			EV << "Queue empty, checking again at: " << startTimeComSink2 << " s" << endl;
			scheduleAt(startTimeComSink2 , checkQueue);
		} else {
			queueElementCounter ++;
			EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
			EV << "Next queue random transmission at : " << randomQueueTime[queueElementCounter] << " s" << endl;
			scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: " << msg->getKind() << endl;
		delete msg;
		msg = 0;
	}
}

void ComputerAppLayer::handleLowerMsg(cMessage *msg)
{
	// Get control info attached by base class decapsMsg method to get RSSI and BER
	assert(dynamic_cast<NetwControlInfo*>(msg->getControlInfo()));
	NetwControlInfo* cInfo = static_cast<NetwControlInfo*>(msg->getControlInfo());
	EV << "The RSSI in Appl Layer is: " << cInfo->getRSSI() << endl;
	EV << "The BER in Appl Layer is: " << cInfo->getBitErrorRate() << endl;

	// Pointer to the source host
	host = cc->findNic(simulation.getModule((static_cast<ApplPkt*>(msg))->getSrcAddr())->getParentModule()->findSubmodule("nic"));

	switch(msg->getKind())
    {
	// The computer treats just reports, it doesn't process any broadcast
    case AppLayer::REPORT_WITHOUT_CSMA:
    case AppLayer::REPORT_WITH_CSMA:
		switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
		{
		case AppLayer::SYNC_PHASE_1:
		case AppLayer::SYNC_PHASE_2:
		case AppLayer::SYNC_PHASE_3:
			EV << "Here we should not receive any Report, just Broadcasts from the AN" << endl;
			numOfPcktsRcvdInWrongPhase++;
			delete msg;
			msg = 0;
			break;
		case AppLayer::REPORT_PHASE:
		case AppLayer::VIP_PHASE:
    		if (host->moduleType == 2) { // Mobile Node
    			// The computer should not receive packets from any Mobile Node
    			error ("The computer cannot receive reports from a Mobile Node");
    			delete msg;
    			msg = 0;
			} else { // Anchor or computer
    			// Don't do anything an Anchor would not communicate with another Anchor in this phases
				error ("The MAC should have discarded this packet, if an AN transmits in this phases is for a Mobile Node");
    			delete msg;
    			msg = 0;
    		}
			break;
		case AppLayer::COM_SINK_PHASE_1:
		case AppLayer::COM_SINK_PHASE_2:
    		if (host->moduleType == 2) { // Mobile Node
    			error ("We cannot receive in this phase any packet from a Mobile Node");
    			delete msg;
    			msg = 0;
    		} else { // Anchor
				if ((static_cast<ApplPkt*>(msg))->getDestAddr() == getParentModule()->findSubmodule("nic")) {
					if ((static_cast<ApplPkt*>(msg))->getRealDestAddr() == getParentModule()->findSubmodule("nic")) {
						// The packet its for me, communication between from an Anchor
						getParentModule()->bubble("I've received the message");
						// Here the Computer would do all the calculations to answer the anchor or the mobile node through an AN
						// Now we interchange destination and source to send it back to the mobile node
						int realSrc = (static_cast<ApplPkt*>(msg))->getRealDestAddr();
						int src = (static_cast<ApplPkt*>(msg))->getDestAddr();
						(static_cast<ApplPkt*>(msg))->setRealDestAddr((static_cast<ApplPkt*>(msg))->getRealSrcAddr());
						(static_cast<ApplPkt*>(msg))->setDestAddr((static_cast<ApplPkt*>(msg))->getSrcAddr());
						(static_cast<ApplPkt*>(msg))->setRealSrcAddr(realSrc);
						(static_cast<ApplPkt*>(msg))->setSrcAddr(src);
						(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
						EV << "Enqueing  packet" << endl;
						packetsQueue.insert(msg);
					} else {
						// This case won't happen, a packet for the computer will be always in real for the computer
		    			delete msg;
		    			msg = 0;
					}
				} else { // The computer doesn't route the packet
					// In this case we will not have this situation because the AN cannot communicate with each other,
					// they communicate with the computer and then this sends the packet to the other AN
	    			delete msg;
	    			msg = 0;
				}
    		}
			break;
		}
		break;
    default:
    	// Here we could program if we need to process the sync packets
    	delete msg;
    	msg = 0;
    }
}

void ComputerAppLayer::handleLowerControl(cMessage *msg)
{
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		nbPacketDroppedBackOffAN++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum BackOff periods, ";
		if ((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() < maxRetransDroppedBackOff) {
			(static_cast<ApplPkt*>(msg))->setRetransmisionCounter((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() + 1);
			if ((static_cast<ApplPkt*>(msg))->getDestAddr() == destination) { // Its a broadcast
				EV << " Broadcast retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedBackOff;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITH_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITHOUT_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITHOUT_CSMA);
				}
			} else { // Its a Report
				EV << " Report retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedBackOff;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report with CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report without CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITHOUT_CSMA);
				}
			}
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::COM_SINK_PHASE_2: // In Computer case, it only transmits in this phase
				sendDown(msg);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeAN++;
				delete msg;
				msg = 0;
				break;
			}
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			delete msg;
			msg = 0;
		}
		EV << endl;
		break;
	case BaseMacLayer::PACKET_DROPPED: // In case its dropped due to no ACK received...
		nbPacketDroppedReportAN++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if ((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() < maxRetransDroppedReportAN) {
			(static_cast<ApplPkt*>(msg))->setRetransmisionCounter((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() + 1);
			if ((static_cast<ApplPkt*>(msg))->getDestAddr() == destination) { // Its a broadcast (it cannot be, but just in cas)
				EV << " Broadcast retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedReportAN;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITH_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITHOUT_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITHOUT_CSMA);
				}
			} else { // Its a Report
				EV << " Report retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedReportAN;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report with CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report without CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITHOUT_CSMA);
				}
			}
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::COM_SINK_PHASE_2: // In Computer case, it only transmits in this phase
				sendDown(msg);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeAN++;
				delete msg;
				msg = 0;
				break;
			}
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			delete msg;
			msg = 0;
		}
		EV << endl;
		break;
	case BaseMacLayer::QUEUE_FULL:
		nbPacketDroppedReportAN++;
		EV << "The queue in MAC is full, discarding the message" << endl;
		delete msg;
		msg = 0;
		break;
	default:
		EV << "Message correctly transmitted." << endl;
		delete msg;
		msg = 0;
		break;
	}
}

int ComputerAppLayer::hasSlot(int *slots, int *slotCounter, int numTotalSlots, int anchor, int numAnchors)
{
	int presenceInSlots = 0;
	for (int i = 0; i < numTotalSlots; i++)
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
