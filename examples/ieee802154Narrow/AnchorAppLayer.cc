#include "AnchorAppLayer.h"

Define_Module(AnchorAppLayer);

void AnchorAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if(stage == 0) {
		// Parameter load
		syncInSlot = par("syncInSlot");
		phaseRepetitionNumber = par("phaseRepetitionNumber");
	   	//syncFirstMaxRandomTime = par("syncFirstMaxRandomTime");
		// Now the first maximum interpacket transmission time is the same as the rest of the times, it could be different, that's why we have 2 parameters
		syncFirstMaxRandomTime = par("syncRestMaxRandomTimes");
	   	syncRestMaxRandomTimes = par("syncRestMaxRandomTimes");

	   	// Variable initialization, we could change this into parameters if necessary
	   	syncPhaseNumber = SYNC_PHASE_1;
		syncPacketsPerSyncPhaseCounter = 1;
		scheduledSlot = 0;
		broadcastCounter = (int*)calloc(sizeof(int), numberOfNodes);

	} else if (stage == 1) {
		// Assign the type of host to 1 (anchor)
		anchor = cc->findNic(myNetwAddr);
		anchor->moduleType = 1;
	// We have to wait till stage 4 to make this because till stage 3 the slot configuration is not done
	} else if (stage == 4) {
		// Necessary variables for the queue initialization
		checkQueue = new cMessage("transmit queue elements", CHECK_QUEUE);
		queueElementCounter = 0;
		maxQueueElements = 30;

		// Broadcast message, slotted or not is without CSMA, we wait the random time in the Appl Layer
		delayTimer = new cMessage("sync-delay-timer", SEND_SYNC_TIMER_WITHOUT_CSMA);
	}
}

AnchorAppLayer::~AnchorAppLayer() {
	cancelAndDelete(delayTimer);
	cancelAndDelete(checkQueue);
	cancelAndDelete(beginPhases);
	for(cQueue::Iterator iter(packetsQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
	for(cQueue::Iterator iter(transfersQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
}


void AnchorAppLayer::finish()
{
	recordScalar("Dropped Packets in AN - No ACK received", nbPacketDroppedNoACK);
	recordScalar("Dropped Packets in AN - Max MAC BackOff tries", nbPacketDroppedBackOff);
	recordScalar("Dropped Packets in AN - App Queue Full", nbPacketDroppedAppQueueFull);
	recordScalar("Dropped Packets in AN - Mac Queue Full", nbPacketDroppedMacQueueFull);
	recordScalar("Dropped Packets in AN - No Time in the Phase", nbPacketDroppedNoTimeApp);
	recordScalar("Erased Packets in AN - No more BackOff retries", nbErasedPacketsBackOffMax);
	recordScalar("Erased Packets in AN - No more No ACK retries", nbErasedPacketsNoACKMax);
	recordScalar("Erased Packets in AN - No more MAC Queue Full retries", nbErasedPacketsMacQueueFull);
	recordScalar("Number of AN Broadcasts Successfully sent", nbBroadcastPacketsSent);
	recordScalar("Number of AN Reports with ACK", nbReportsWithACK);
	recordScalar("Number of Broadcasts received in AN", nbBroadcastPacketsReceived);
	recordScalar("Number of Reports received in AN", nbReportsReceived);
	recordScalar("Number of Reports really for me received in AN", nbReportsForMeReceived);
}

void AnchorAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case SEND_SYNC_TIMER_WITHOUT_CSMA:
		sendBroadcast(); // Send the broadcast
		// If we still have full phases to do
		if (syncInSlot) // Slotted mode
		{
			scheduledSlot++;
			if (scheduledSlot < anchor->numSlots) { // If an Anchor has more than one slot per slot period (reuse of slots), first we assign all the slots
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
			} else { // Calculate the first time of the next slot period
				scheduledSlot = 0; // Reset of the scheduled slots in this slot period (when more than one slot per Anchor)
				if (syncPacketsPerSyncPhaseCounter < syncPacketsPerSyncPhase) {
					syncPacketsPerSyncPhaseCounter++; // Increments the mini sync phases covered from a total of syncPacketsPerSyncPhase
					nextPhaseStart = nextPhaseStart + (timeSyncPhase / syncPacketsPerSyncPhase); // Here we add the time of a mini sync phase (another slot period)
					scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
					EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				} else { // If we reached all the slot periods (mini sync phase) from a sync phase, don't schedule any more, in next sync phase all will start again
					syncPacketsPerSyncPhaseCounter = 1;
				}
			}
		}
		else
		{
			// ---------------------------------------------------------------------------
			// - Here we would manage the sync packets when they are not in slotted mode -
			// ---------------------------------------------------------------------------
		}
		break;
	case CHECK_QUEUE:
		if (packetsQueue.length() > 0) {
			cMessage * msg = (cMessage *)packetsQueue.pop();
			EV << "Sending packet from the Queue to Anchor with addr. " << static_cast<ApplPkt *>(msg)->getDestAddr() << endl;
			transfersQueue.insert(msg->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(msg);
			if (packetsQueue.length() > 0) { // We have to check again if the queue has still elements after taking the element previously
				// Schedule the next queue element in the next random time
				queueElementCounter++;
				EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
				EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
				scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
			}
		}
		break;
	case BEGIN_PHASE:
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
		switch (nextPhase)
		{
		case AppLayer::SYNC_PHASE_1:
			phase = AppLayer::SYNC_PHASE_1;
			nextPhase = AppLayer::REPORT_PHASE;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)
			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			break;
		case AppLayer::REPORT_PHASE:
			phase = AppLayer::REPORT_PHASE;
			nextPhase = AppLayer::VIP_PHASE;
			nextPhaseStartTime = simTime() + timeReportPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::VIP_PHASE:
			phase = AppLayer::VIP_PHASE;
			nextPhase = AppLayer::SYNC_PHASE_2;
			nextPhaseStartTime = simTime() + timeVIPPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::SYNC_PHASE_2:
			phase = AppLayer::SYNC_PHASE_2;
			nextPhase = AppLayer::COM_SINK_PHASE_1;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			for (int i = 0; i < numberOfNodes; i++) {
				if (broadcastCounter[i] > 0) { // If the AN has received at least one Broadcast
					ApplPkt *pkt = new ApplPkt("Report with CSMA", REPORT_WITH_CSMA);
					pkt->setBitLength(bcastMixANPacketLength);
					//pkt->setBitLength(88);
					pkt->setRealDestAddr(getParentModule()->getParentModule()->getSubmodule("computer", 0)->findSubmodule("nic"));
					pkt->setDestAddr(pkt->getRealDestAddr());
					pkt->setSrcAddr(myNetwAddr);
					pkt->setRealSrcAddr(getParentModule()->getParentModule()->getSubmodule("node", i)->findSubmodule("nic"));
					pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
					pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
					pkt->setCSMA(true);
					if (packetsQueue.length() < maxQueueElements) { // There is still place in the queue for more packets
						EV << "Enqueing broadcast RSSI values from Mobile Node " << i << endl;
						packetsQueue.insert(pkt);
					} else {
						EV << "Queue full, discarding packet" << endl;
						nbPacketDroppedAppQueueFull++;
						delete pkt;
					}
				}
			}
			broadcastCounter = (int*)calloc(sizeof(int), numberOfNodes); // Reset the counter of broadcast a AN received from Mobile Nodes
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)
			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			break;
		case AppLayer::COM_SINK_PHASE_1:
			phase = AppLayer::COM_SINK_PHASE_1;
			nextPhase = AppLayer::SYNC_PHASE_3;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// At the beginning of the Com Sink 1 the Anchor checks its queue to transmit the elements and calculate all the random transmission times
			if (packetsQueue.length() > 0) { // Only if the Queue has elements we do calculate all the intermediate times
				stepTimeComSink1 = (timeComSinkPhase - guardTimeComSinkPhase) / packetsQueue.length();
				randomQueueTime = (simtime_t*)calloc(sizeof(simtime_t), packetsQueue.length());
				EV << "Transmitting the " << packetsQueue.length() << " elements of the queue in the following moments." << endl;
				for (int i = 0; i < packetsQueue.length(); i++) {
					randomQueueTime[i] = simTime() + (i * stepTimeComSink1) + uniform(0, stepTimeComSink1, 0);
					EV << "Time " << i << ": " << randomQueueTime[i] << endl;
				}
				queueElementCounter = 0; // Reset the index to know which random time from vector to use
				EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
				EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
				scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
			} else {
				EV << "Queue empty, Anchor has nothing to communicate this full phase (period)." << endl;
			}

			break;
		case AppLayer::SYNC_PHASE_3:
			phase = AppLayer::SYNC_PHASE_3;
			nextPhase = AppLayer::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)
			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			if (phaseRepetitionNumber > 0) {
				phaseRepetitionNumber--; // If the number of full phases is limited, decrease one as we just finished one
			}
			break;
		case AppLayer::COM_SINK_PHASE_2:
			phase = AppLayer::COM_SINK_PHASE_2;
			nextPhase = AppLayer::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}


void AnchorAppLayer::handleLowerMsg(cMessage *msg)
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
	switch(phase)
	{
	case AppLayer::SYNC_PHASE_1:
	case AppLayer::SYNC_PHASE_2:
	case AppLayer::SYNC_PHASE_3:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Report from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Report from a Anchor" << endl;
	    	}
			delete msg;
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		// --------------------------------------------------------------------------------------
	    		// - If we need to do something with the sync packets from the Anchors, it will be here -
	    		// --------------------------------------------------------------------------------------
	    		EV << "Discarding the packet, Anchor doesn't make anything with Sync Broadcast packets from the AN" << endl;
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
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
	    		nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { //The packet is for me
					if (pkt->getRealDestAddr() == myNetwAddr) { //The packet is really for me
						nbReportsForMeReceived++;
						// The packet its for me, probably Distributed-A, or request for configuration or info
						if (pkt->getRequestPacket()) { // We are in a request process, we received a request packet
							// -------------------------------------------------------------------------------------------
							// - Here we could write a conditional to send the info from a queue where we would have it --
							// - or a packet saying there is no info available yet, we will just send an standard packet -
							// -------------------------------------------------------------------------------------------
							// Change the packet info to answer the mobile node
							EV << "The Anchor answers immediately to the Mobile Node Request packet" << endl;
							pkt->setBitLength(answerANtoMNPacketLength);
							//pkt->setBitLength(88);
							pkt->setDestAddr(pkt->getSrcAddr());
							pkt->setRealDestAddr(pkt->getRealSrcAddr());
							pkt->setSrcAddr(myNetwAddr);
							pkt->setRealSrcAddr(myNetwAddr);
							pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
							pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
							pkt->setCSMA(true);
							pkt->setRequestPacket(false);
							pkt->setAskForRequest(false);

							transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
							sendDown(pkt);
							break;
						} else {
							EV << "The Anchor received a petition for him but not a Request, Mobile Node is in Distributed A mode" << endl;
							// The packet is a Distributed A packet, calculate position and put a message into the queue for the mobile node to wait when it asks
							// ----------------------------------------------------------------------------------------------------------------------------
							// - Here we have to calculate the position and put it into a queue, not delete the message, but in our case is not important -
							// ----------------------------------------------------------------------------------------------------------------------------
							delete msg;
						}
					}
					else
					{
						// The packet is probably for the computer, Centralized, we assign the dest. addr the computer addr and put it in the queue to transmit
						// We change also the src addr for the addr from the AN, so the next AN can find it in the routing table
						pkt->setDestAddr(pkt->getRealDestAddr());
						pkt->setSrcAddr(myNetwAddr);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						pkt->setCSMA(true);
						pkt->setRequestPacket(false);
						pkt->setAskForRequest(false);
						if (packetsQueue.length() < maxQueueElements) { // There is still place in the queue for more packets
							EV << "The packet is not really for me, is for the computer, I put it in the queue to send it in next Com Sink 1 to the computer" << endl;
							packetsQueue.insert(pkt);
						} else {
							EV << "The packet is not really for me, is for the computer, but Queue full, discarding packet" << endl;
							nbPacketDroppedAppQueueFull++;
							delete msg;
						}
					}
				} else {
					// Don't do anything if a Mobile Node sent a Report and its not for me
					EV << "Discarding the packet, the MAC should have discarded the packet as is not for me" << endl;;
	    			delete msg;
				}
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Report or VIP phases Anchor cannot receive any Report from an Anchor" << endl;
	    		delete msg;
	    	}
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				// If we receive a Broadcast Packet from a Mobile Node, we have to check if we received the last one in this phase
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
				// - We should make here an array to store all the RSSI values we read from the broadcast to include them in the packet we send at the next Com Sink 1 -
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
				// The computer will analyze all the received packets from different AN to estimate the position
	    		nbBroadcastPacketsReceived++;
	    		indexBroadcast = simulation.getModule(pkt->getSrcAddr())->getParentModule()->getIndex();
				broadcastCounter[indexBroadcast]++;
				EV << "Received the Broadcast Packet number " << broadcastCounter[indexBroadcast] << ". From the Mobile Node with Addr: " << pkt->getSrcAddr() << endl;
				delete msg;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Report or VIP phases Anchor cannot receive a Broadcast from any Anchor" << endl;
				delete msg;
	    	}
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
	    }
		break;
	case AppLayer::COM_SINK_PHASE_1:
	case AppLayer::COM_SINK_PHASE_2:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Report from a Mobile Node" << endl;
				delete msg;
	    	} else { // Computer or AN
	    		nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { // The packet is for me
					if (pkt->getRealDestAddr() == myNetwAddr) { // The packet is really for me
						nbReportsForMeReceived++;
						// Communication computer - anchor or anchor - computer (the anchor - anchor communication is not happening in our study)
						EV << "The computer wanted to say something to this Anchor, but not for a Mobile Node" << endl;
						getParentModule()->bubble("I've received the message");
					} else { // The destination is a mobile node, we put the message in a queue till the mobile node requests for it
						pkt->setDestAddr(pkt->getRealDestAddr());
						pkt->setSrcAddr(myNetwAddr);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						// The packet is probably for a mobile node, answer from the computer, we assign the dest. addr the mobile node addr and transmit it directly
						EV << "We received a packet for a mobile node, we have to save it until it is requested" << endl;
						// ------------------------------------------------------------------------------------------------------------------------
						// - Here we would not delete the message, we would put it into a queue to have it there when the Mobile Node asks for it -
						// ------------------------------------------------------------------------------------------------------------------------
					}
					delete msg;
				} else { // If the message is not for us, we just route the packet
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						EV << "Origen: " << pkt->getSrcAddr() << endl;
						EV << "Origen real: " << pkt->getRealSrcAddr() << endl;
						EV << "Destino: " << pkt->getDestAddr() << endl;
						EV << "Destino real: " << pkt->getRealDestAddr() << endl;
						EV << "Tipo: " << pkt->getKind() << endl;
						EV << "Nombre: " << pkt->getName() << endl;
						EV << "CSMA: " << pkt->getCSMA() << endl;
						transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
						sendDown(pkt);
						break;
				}
	    	}
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Broadcast from an Anchor" << endl;
	    	}
			delete msg;
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
	    }
		break;
	default:
		EV << "Unknown Phase! -> deleting packet, kind: " << msg->getKind() << endl;
		delete msg;
	}
	delete cInfo;
}


void AnchorAppLayer::handleLowerControl(cMessage *msg)
{
	ApplPkt* pkt;
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedBackOff++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum BackOff periods, ";
		if (pkt->getRetransmisionCounterBO() < maxRetransDroppedBackOff) {
			pkt->setRetransmisionCounterBO(pkt->getRetransmisionCounterBO() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterBO() << " of " << maxRetransDroppedBackOff;
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsBackOffMax++;
			delete pkt;
		}
		EV << endl;
		break;
	case BaseMacLayer::PACKET_DROPPED: // In case its dropped due to no ACK received...
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedNoACK++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if (pkt->getRetransmisionCounterACK() < maxRetransDroppedReportAN) {
			pkt->setRetransmisionCounterACK(pkt->getRetransmisionCounterACK() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterACK() << " of " << maxRetransDroppedReportAN;
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
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
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "The Broadcast packet was successfully transmitted into the air" << endl;
		nbBroadcastPacketsSent++;
		delete pkt;
		break;
	case BaseMacLayer::TX_OVER:
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "Message correctly transmitted, received the ACK." << endl;
		nbReportsWithACK++;
		delete pkt;
		break;
	case BaseMacLayer::ACK_SENT:
		EV << "ACK correctly sent" << endl;
		break;
	}
	delete msg;
}

void AnchorAppLayer::sendBroadcast()
{
	// It doesn't matter if we have slotted version or not, CSMA must be disabled, we control random time and retransmission in Appl layer
	ApplPkt *pkt = new ApplPkt("SYNC_MESSAGE_WITHOUT_CSMA", SYNC_MESSAGE_WITHOUT_CSMA);
	pkt->setBitLength(syncPacketLength);
	//pkt->setBitLength(88);

	pkt->setSrcAddr(myNetwAddr);
	pkt->setRealSrcAddr(myNetwAddr);
	pkt->setDestAddr(destination);
	pkt->setRealDestAddr(destination);
	pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
	pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
	pkt->setCSMA(false);

	EV << "Inserting Broadcast Packet in Transmission Queue" << endl;
	transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
	sendDown(pkt);
}
