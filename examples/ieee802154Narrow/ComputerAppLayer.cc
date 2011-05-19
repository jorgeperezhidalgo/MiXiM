#include "ComputerAppLayer.h"

Define_Module(ComputerAppLayer);

void ComputerAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if (stage == 1) {
		// Assign the type of host to 3 (computer)
		computer = cc->findNic(myNetwAddr);
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
		// -----------------------------------------------------------------------------------
		// - To have a more real system, here the computer would send packets to the Anchors -
		// - with the slot configuration, we just configured the directly --------------------
		// -----------------------------------------------------------------------------------
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
		maxQueueElements = 200;
	}
}

ComputerAppLayer::~ComputerAppLayer() {
	cancelAndDelete(checkQueue);
	cancelAndDelete(beginPhases);
	for(cQueue::Iterator iter(packetsQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
	for(cQueue::Iterator iter(transfersQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
}

void ComputerAppLayer::finish()
{
	recordScalar("Dropped Packets in Comp - No ACK received", nbPacketDroppedNoACK);
	recordScalar("Dropped Packets in Comp - Max MAC BackOff tries", nbPacketDroppedBackOff);
	recordScalar("Dropped Packets in Comp - App Queue Full", nbPacketDroppedAppQueueFull);
	recordScalar("Dropped Packets in Comp - Mac Queue Full", nbPacketDroppedMacQueueFull);
	recordScalar("Dropped Packets in Comp - No Time in the Phase", nbPacketDroppedNoTimeApp);
	recordScalar("Erased Packets in Comp - No more BackOff retries", nbErasedPacketsBackOffMax);
	recordScalar("Erased Packets in Comp - No more No ACK retries", nbErasedPacketsNoACKMax);
	recordScalar("Erased Packets in Comp - No more MAC Queue Full retries", nbErasedPacketsMacQueueFull);
	recordScalar("Number of Comp Reports with ACK", nbReportsWithACK);
	recordScalar("Number of Reports received in Comp", nbReportsReceived);
	recordScalar("Number of Reports really for me received in Comp", nbReportsForMeReceived);
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
					EV << "Tiempo " << i << ": " << randomQueueTime[i] << endl;
				}
			}
			startTimeComSink2 = simTime() + fullPhaseTime; // Calculate now the start of the next Com Sink 2 phase
		} else {
			cMessage * msg = (cMessage *)packetsQueue.pop();
			EV << "Sending packet from the Queue" << endl;
			// Prior to send the message, we check that we are still in the Com Sink 2 Phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::COM_SINK_PHASE_2: // In Computer case, it only transmits in this phase
				transfersQueue.insert(msg->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
				sendDown(msg);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeApp++;
				delete msg;
			}
		}
		// Calculates the next time this selfmessage will be scheduled, in this fullphase (still elements in queue) or in the next one
		if (packetsQueue.length() == 0) {
			queueElementCounter = -1;
			EV << "Queue empty, checking again at: " << startTimeComSink2 << " s" << endl;
			scheduleAt(startTimeComSink2 , checkQueue);
		} else {
			queueElementCounter ++;
			EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
			EV << "Next queue random transmission at : " << randomQueueTime[queueElementCounter] << " s" << queueElementCounter << endl;
			scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
		}
		break;
	case BEGIN_PHASE:
		switch (nextPhase)
		{
		case AppLayer::SYNC_PHASE_1:
			nextPhase = AppLayer::REPORT_PHASE;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::REPORT_PHASE:
			nextPhase = AppLayer::VIP_PHASE;
			nextPhaseStartTime = simTime() + timeReportPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::VIP_PHASE:
			nextPhase = AppLayer::SYNC_PHASE_2;
			nextPhaseStartTime = simTime() + timeVIPPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::SYNC_PHASE_2:
			nextPhase = AppLayer::COM_SINK_PHASE_1;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::COM_SINK_PHASE_1:
			nextPhase = AppLayer::SYNC_PHASE_3;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::SYNC_PHASE_3:
			nextPhase = AppLayer::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::COM_SINK_PHASE_2:
			nextPhase = AppLayer::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		}
		// Empty the transmission Queue
		if (!transfersQueue.empty()) {
			EV << "Emptying the queue with " << transfersQueue.length() << " elements in phase change" << endl;
			nbPacketDroppedNoTimeApp = nbPacketDroppedNoTimeApp + transfersQueue.length();
			transfersQueue.clear();
			// --------------------------------------------------------------------------------------
			// - If we don't want to clear the queue and lose the packets, --------------------------
			// - we could somehow here leave the rest of the queue for the next full phase (period) -
			// --------------------------------------------------------------------------------------
		} else {
			EV << "App Transmission Queue empty in phase change." << endl;
		}
		break;
	default:
		EV << "Unknown selfmessage! -> delete, kind: " << msg->getKind() << endl;
		delete msg;
	}
}

void ComputerAppLayer::handleLowerMsg(cMessage *msg)
{
	// Convert the message to an Appl Packet
	ApplPkt* pkt = check_and_cast<ApplPkt*>(msg);
	EV << "Received packet from (" << pkt->getSrcAddr() << ", " << pkt->getRealSrcAddr() << ") to (" << pkt->getDestAddr() << ", " << pkt->getRealDestAddr() << ") with Name: " << pkt->getName() << endl;

	// Get control info attached by base class decapsMsg method to get RSSI and BER
	assert(dynamic_cast<NetwControlInfo*>(pkt->getControlInfo()));
	NetwControlInfo* cInfo = static_cast<NetwControlInfo*>(pkt->removeControlInfo());
	EV << "The RSSI in Appl Layer is: " << cInfo->getRSSI() << endl;
	EV << "The BER in Appl Layer is: " << cInfo->getBitErrorRate() << endl;

	// Pointer to the source host
	host = cc->findNic(pkt->getSrcAddr());

	// Filter first according to the phase we are in
	switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
	{
	case AppLayer::SYNC_PHASE_1:
	case AppLayer::SYNC_PHASE_2:
	case AppLayer::SYNC_PHASE_3:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase computer cannot receive any Report from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Sync Phase computer cannot receive any Report from a Anchor" << endl;
	    	}
			delete msg;
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase computer cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		// --------------------------------------------------------------------------------------
	    		// - If we need to do something with the sync packets from the Anchors, it will be here -
	    		// --------------------------------------------------------------------------------------
	    		EV << "Discarding the packet, computer doesn't make anything with Sync Broadcast packets from the AN" << endl;
	    	}
			delete msg;
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
	    }
		break;
	case AppLayer::REPORT_PHASE:
	case AppLayer::VIP_PHASE:
		EV << "Discarding the packet, computer doesn't receive any packet during the Report and VIP phases" << endl;
		delete msg;
		break;
	case AppLayer::COM_SINK_PHASE_1:
		if (host->moduleType == 2) { // Mobile Node
			EV << "Discarding the packet, computer cannot receive in Com Sink 1 Phase any packet from a Mobile Node" << endl;
			delete msg;
		} else { // Anchor
			switch(pkt->getKind())
		    {
		    case AppLayer::REPORT_WITHOUT_CSMA:
		    case AppLayer::REPORT_WITH_CSMA:
		    	nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { // If the packet is for the computer
					if (pkt->getRealDestAddr() == myNetwAddr) { // If the packet was really for the computer
						nbReportsForMeReceived++;
						// The packet its for the computer, communication received from an Anchor
						getParentModule()->bubble("I've received the message");
						// ---------------------------------------------------------------------------------------------------------
						// - Here the Computer would do all the calculations to answer the anchor or the mobile node through an AN -
						// ---------------------------------------------------------------------------------------------------------
						// Now we interchange destination and source to send it back to the mobile node
						int realSrc = pkt->getRealDestAddr();
						int src = pkt->getDestAddr();
						pkt->setRealDestAddr(pkt->getRealSrcAddr());
						pkt->setDestAddr(pkt->getSrcAddr());
						pkt->setRealSrcAddr(realSrc);
						pkt->setSrcAddr(src);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						pkt->setCSMA(true);
						if (packetsQueue.length() < maxQueueElements) { // There is still place in the queue for more packets
							EV << "Enqueing  packet" << endl;
							packetsQueue.insert(pkt);
							EV << "Origen: " << pkt->getSrcAddr() << endl;
							EV << "Origen real: " << pkt->getRealSrcAddr() << endl;
							EV << "Destino: " << pkt->getDestAddr() << endl;
							EV << "Destino real: " << pkt->getRealDestAddr() << endl;
							EV << "Tipo: " << pkt->getKind() << endl;
							EV << "Nombre: " << pkt->getName() << endl;
							EV << "CSMA: " << pkt->getCSMA() << endl;
						} else {
							EV << "Queue full, discarding packet" << endl;
							nbPacketDroppedAppQueueFull++;
							delete msg;
						}
					} else {
						// This case won't happen, a packet for the computer will be always in real for the computer
						EV << "Discarding packet, the computer received a packet that was not really for him, for us not possible" << endl;
		    			delete msg;
					}
				} else { // The computer doesn't route the packet
					// In this case we will not have this situation because the AN cannot communicate with each other,
					// they communicate with the computer and then this sends the packet to the other AN
					EV << "Discarding packet, the computer received a packet for another Anchor, and the computer does not route packets" << endl;
	    			delete msg;
				}
				break;
		    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
		    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
		    	EV << "Discarding Packet, computer won't receive any Sync Broadcast Packet during Com Sink 1 Phase" << endl;
		    	delete msg;
		    	break;
		    default:
				EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
				delete msg;
		    }
		}
		break;
	case AppLayer::COM_SINK_PHASE_2:
		EV << "Discarding packet, the computer doesn't receive any packet during Com Sink 2 Phase" << endl;
		delete msg;
		break;
	default:
		EV << "Unknown Phase! -> deleting packet, kind: " << msg->getKind() << endl;
		delete msg;
	}
	delete cInfo;
}

void ComputerAppLayer::handleLowerControl(cMessage *msg)
{
	ApplPkt* pkt;
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedBackOff++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum BackOff periods, ";
		if (pkt->getRetransmisionCounterBO() < maxRetransDroppedBackOff) {
			pkt->setRetransmisionCounterBO(pkt->getRetransmisionCounterBO() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterBO() << " of " << maxRetransDroppedBackOff;
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::COM_SINK_PHASE_2: // In Computer case, it only transmits in this phase
				transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
				sendDown(pkt);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeApp++;
				delete pkt;
			}
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsBackOffMax++;
			delete pkt;
		}
		EV << endl;
		break;
	case BaseMacLayer::PACKET_DROPPED: // In case its dropped due to no ACK received...
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedNoACK++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if (pkt->getRetransmisionCounterACK() < maxRetransDroppedReportAN) {
			pkt->setRetransmisionCounterACK(pkt->getRetransmisionCounterACK() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterACK() << " of " << maxRetransDroppedReportAN;
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::COM_SINK_PHASE_2: // In Computer case, it only transmits in this phase
				transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
				sendDown(pkt);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeApp++;
				delete pkt;
				break;
			}
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsNoACKMax++;
			delete pkt;
		}
		EV << endl;
		break;
	case BaseMacLayer::QUEUE_FULL:
		// Take the last message from the transmission queue, the last because as the mac queue is full the Mac queue never had this packet
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.remove(transfersQueue.back()));
		EV << "The queue in MAC is full, discarding the message" << endl;
		nbPacketDroppedMacQueueFull++;
		nbErasedPacketsMacQueueFull++;
		delete pkt;
		break;
	case BaseMacLayer::SYNC_SENT:
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "The Broadcast packet was successfully transmitted into the air" << endl;
		nbBroadcastPacketsSent++;
		delete pkt;
		break;
	case BaseMacLayer::TX_OVER:
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "Message correctly transmitted, received the ACK." << endl;
		nbReportsWithACK++;
		delete pkt;
		break;
	}
	delete msg;
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
