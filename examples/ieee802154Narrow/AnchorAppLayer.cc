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

	} else if (stage == 1) {
		// Assign the type of host to 1 (anchor)
		anchor = cc->findNic(getParentModule()->findSubmodule("nic"));
		anchor->moduleType = 1;
	// We have to wait till stage 4 to make this because till stage 3 the slot configuration is not done
	} else if (stage == 4) {
		// Schedule the first queue processing at the start of the Com Sink 1
		checkQueue = new cMessage("transmit queue elements", CHECK_QUEUE);
		startTimeComSink1 = simTime() + timeSyncPhase + timeReportPhase + timeVIPPhase + timeSyncPhase;
		scheduleAt(startTimeComSink1 , checkQueue);
		queueElementCounter = -1;

		// Broadcast message, slotted or not is without CSMA, we wait the random time in the Appl Layer
		delayTimer = new cMessage("sync-delay-timer", SEND_SYNC_TIMER_WITHOUT_CSMA);

		// If we execute some full phase
		if (phaseRepetitionNumber != 0 && syncInSlot) // If sync phase slotted
		{
			nextSyncSend = simTime();
			scheduleAt(nextSyncSend + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
			scheduledSlot++;
		}
		else if (phaseRepetitionNumber != 0) // If sync phase with random transmissions
		{
			scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
		}
	}
}

AnchorAppLayer::~AnchorAppLayer() {
	cancelAndDelete(delayTimer);
	cancelAndDelete(checkQueue);
}


void AnchorAppLayer::finish()
{
	recordScalar("dropped_AN", nbPacketDroppedReportAN);
	recordScalar("dropped_backoff_AN", nbPacketDroppedBackOffAN);
	recordScalar("dropped_no_time_in_AN", nbPacketDroppedNoTimeAN);
	recordScalar("packets_rcv_in_wrong_phase", numOfPcktsRcvdInWrongPhase);
}

void AnchorAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case SEND_SYNC_TIMER_WITHOUT_CSMA:
		assert(msg == delayTimer);

		sendBroadcast();

		// If we still have full phases to do
		if (phaseRepetitionNumber != 0 && syncInSlot) // Slotted mode
		{
			// If an Anchor has more than an slot per slot period (reuse os slots), then first we assign all the slots
			// and when finish we enter this "if" to calculate the first time of the next slot period
			if (scheduledSlot >= anchor->numSlots)
			{
				scheduledSlot = 0; // Reset of the scheduled slots in this slot period (when more than one slot per Anchor)
				syncPacketsPerSyncPhaseCounter++; // Increments the mini sync phases covered from a total of syncPacketsPerSyncPhase
				nextPhaseStart = nextPhaseStart + (timeSyncPhase / syncPacketsPerSyncPhase); // Here we add the time of a mini sync phase (another slot period)
				if (syncPacketsPerSyncPhaseCounter > syncPacketsPerSyncPhase) // If we reached all the slot periods (mini sync phase) from a sync phase, we add the time between syncphases also (Report time, Com Sink time...)
				{
					syncPacketsPerSyncPhaseCounter = 1;
					switch (syncPhaseNumber)
					{
					case SYNC_PHASE_1:
						syncPhaseNumber = SYNC_PHASE_2;
						nextPhaseStart = nextPhaseStart + timeReportPhase + timeVIPPhase;
						break;
					case SYNC_PHASE_2:
						syncPhaseNumber = SYNC_PHASE_3;
						nextPhaseStart = nextPhaseStart + timeComSinkPhase;
						break;
					case SYNC_PHASE_3:
						syncPhaseNumber = SYNC_PHASE_1;
						nextPhaseStart = nextPhaseStart + timeComSinkPhase;
						phaseRepetitionNumber--;
						break;
					default:
						syncPhaseNumber = SYNC_PHASE_1;
					}
				}
			}
			scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
			scheduledSlot++;
			EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
		}
		else
		{

		}
		break;
	case CHECK_QUEUE:
		// If we didn't transmit any element of the queue is the moment to calculate all the random transmission times
		if (queueElementCounter == -1) {
			if (packetsQueue.length() > 0) { // Only if the Queue has elements we do calculate all the intermediate times
				stepTimeComSink1 = (timeComSinkPhase - guardTimeComSinkPhase) / packetsQueue.length();
				randomQueueTime = (simtime_t*)calloc(sizeof(simtime_t), packetsQueue.length());
				for (int i = 0; i < packetsQueue.length(); i++) {
					randomQueueTime[i] = startTimeComSink1 + (i * stepTimeComSink1) + uniform(0, stepTimeComSink1, 0);
				}
			}
			startTimeComSink1 = simTime() + fullPhaseTime; // Calculate now the start of the next Com Sink 1 phase
		} else {
			cMessage * msg = (cMessage *)packetsQueue.pop();
			EV << "Sending packet from the Queue" << endl;
			sendDown(msg);
		}
		// Calculates the next time this selfmessage will be scheduled, in this fullphase (still elements in queue) or in the next one
		if (packetsQueue.length() == 0) {
			queueElementCounter = -1;
			EV << "Queue empty, checking again at: " << startTimeComSink1 << " s" << endl;
			scheduleAt(startTimeComSink1 , checkQueue);
		} else {
			queueElementCounter ++;
			EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
			EV << "Next queue random transmission at : " << randomQueueTime[queueElementCounter] << " s" << endl;
			scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}


void AnchorAppLayer::handleLowerMsg(cMessage *msg)
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
    case AppLayer::REPORT_WITHOUT_CSMA:
    case AppLayer::REPORT_WITH_CSMA:
    	// Pointer to the real destination
    	dest = cc->findNic(simulation.getModule((static_cast<ApplPkt*>(msg))->getRealDestAddr())->getParentModule()->findSubmodule("nic"));
		switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
		{
		case AppLayer::SYNC_PHASE_1:
		case AppLayer::SYNC_PHASE_2:
		case AppLayer::SYNC_PHASE_3:
			EV << "Here we should not receive any Report, just Broadcasts from other AN" << endl;
			numOfPcktsRcvdInWrongPhase++;
			delete msg;
			msg = 0;
			break;
		case AppLayer::REPORT_PHASE:
		case AppLayer::VIP_PHASE:
			EV << "Received a packet in the Report or VIP Phase, ";
    		if (host->moduleType == 2) { // Mobile Node
    			EV << " from a Mobile Node." << endl;
				if ((static_cast<ApplPkt*>(msg))->getDestAddr() == getParentModule()->findSubmodule("nic"))	{
					EV << "The packet is for me" << endl;
					if ((static_cast<ApplPkt*>(msg))->getRealDestAddr() == getParentModule()->findSubmodule("nic")) {
						EV << "The packet is really for me" << endl;
						// The packet its for me, probably Distributed-A, or request for configuration or info
						if ((static_cast<ApplPkt*>(msg))->getRequestPacket()) { // We are in a request process
							EV << "The packet is a request packet" << endl;
							// Here we could write a conditional to send the info from a queue where we would have it
							// or a packet saying there is no info available yet, we will just send a packet, in our cas just send a packet
							// We change the msg packet info to reach the mobile node
							(static_cast<ApplPkt*>(msg))->setBitLength(packetLength);
							int src = (static_cast<ApplPkt*>(msg))->getSrcAddr();
							int realSrc = (static_cast<ApplPkt*>(msg))->getRealSrcAddr();
							(static_cast<ApplPkt*>(msg))->setSrcAddr(getParentModule()->findSubmodule("nic"));
							(static_cast<ApplPkt*>(msg))->setRealSrcAddr(getParentModule()->findSubmodule("nic"));
							(static_cast<ApplPkt*>(msg))->setDestAddr(src);
							(static_cast<ApplPkt*>(msg))->setRealDestAddr(realSrc);
							(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
							(static_cast<ApplPkt*>(msg))->setCSMA(true);
							// Before we send the packet to the Mobile Node, we have to check if we are still in the report phase
							switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
							{
							case AppLayer::REPORT_PHASE: // In Anchor case, it only routes in this 2 phases
							case AppLayer::VIP_PHASE:
								EV << "I'm still in the Report or VIP Phase so I send the packet to the Mobile Node" << endl;
								sendDown(msg);
								break;
							default: // If we are in any of the other phases, we don't retransmit
								EV << "I'm already out of the Report or VIP Phase so I delete de message" << endl;
								nbPacketDroppedNoTimeAN++;
								delete msg;
								msg = 0;
								break;
							}
						} else {
							// The packet is a Distributed A packet, calculate position and put a message into the queue for the mobile node to wait when it asks
							// Here we have to calculate the position and put it into a queue, not delete the message, but in our case is not important
							delete msg;
							msg = 0;
						}
					}
					else
					{
						// The packet is probably for the computer, Centralized, we assign the dest. addr the computer addr and put it in the queue to transmit
						// We change also the src addr for the addr from the AN, so the next AN can find it in the routing table
						(static_cast<ApplPkt*>(msg))->setDestAddr((static_cast<ApplPkt*>(msg))->getRealDestAddr());
						(static_cast<ApplPkt*>(msg))->setSrcAddr(getParentModule()->findSubmodule("nic"));
						(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
						EV << "Enqueing  packet" << endl;
						packetsQueue.insert(msg);
						EV << "The packet is not for me, I put it in the queue to send it in next Com Sink 1 to the computer" << endl;
					}
				}
				else
				{
					// Don't do anything if a Mobile Node sends a Report and its not for me
					error ("The MAC should have discarded this packet");
	    			delete msg;
	    			msg = 0;
				}
    		}
    		else
    		{ // Anchor or computer
    			EV << " from an Anchor or the Computer" << endl;
    			// Don't do anything an Anchor would not communicate with another Anchor in this phases
				EV << "We should not receive any packet from an AN or Computer in this phase" << endl;
				numOfPcktsRcvdInWrongPhase++;
    			delete msg;
    			msg = 0;
    		}
			break;
		case AppLayer::COM_SINK_PHASE_1:
		case AppLayer::COM_SINK_PHASE_2:
    		if (host->moduleType == 2) { // Mobile Node
    			EV << "We cannot receive in this phase any packet from a Mobile Node" << endl;
				numOfPcktsRcvdInWrongPhase++;
    			delete msg;
    			msg = 0;
    		} else { // Anchor or computer
				if ((static_cast<ApplPkt*>(msg))->getDestAddr() == getParentModule()->findSubmodule("nic")) {
					if ((static_cast<ApplPkt*>(msg))->getRealDestAddr() == getParentModule()->findSubmodule("nic")) {
						// The packet its for me, communication computer - anchor or anchor - anchor (the last one is not happening in our study)
						getParentModule()->bubble("I've received the message");
				    	delete msg;
				    	msg = 0;
					} else if (dest->moduleType == 2) { // The destination is a mobile node, we put the message in a queue till the mobile node requests for it
						(static_cast<ApplPkt*>(msg))->setDestAddr((static_cast<ApplPkt*>(msg))->getRealDestAddr());
						(static_cast<ApplPkt*>(msg))->setSrcAddr(getParentModule()->findSubmodule("nic"));
						(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
						// The packet is probably for a mobile node, answer from the computer, we assign the dest. addr the mobile node addr and transmit it directly
						EV << "We received a packet for a mobile node, we have to save it until it is requested" << endl;
						// Here we would not delete the message, we would put it into a queue to have it there when the Mobile Node asks for it
						delete msg ;
						msg = 0;
					}
				} else { // It the message is not for us, we just route the packet
					// When we receive a packet to route, first we have to be sure we are still in the Com Sink Phases in case there wasn't time for all the packets
					switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
					{
					case AppLayer::COM_SINK_PHASE_1: // In Anchor case, it only routes in this 2 phases
					case AppLayer::COM_SINK_PHASE_2:
						(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
						sendDown(msg);
						break;
					default: // If we are in any of the other phases, we don't retransmit
						nbPacketDroppedNoTimeAN++;
						delete msg;
						msg = 0;
						break;
					}
				}
    		}
			break;
		}
		break;
    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
		switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
		{
		case AppLayer::SYNC_PHASE_1:
		case AppLayer::SYNC_PHASE_2:
		case AppLayer::SYNC_PHASE_3:
			// Here we would take info from the Sync Packets from other AN if necessary
			delete msg;
			msg = 0;
			break;
		case AppLayer::REPORT_PHASE:
		case AppLayer::VIP_PHASE:
    		if (host->moduleType == 2) { // Mobile Node
				// If we receive a Broadcast Packet from a Mobile Node, we have to redirect it to the computer
    			// The computer will analize all the received packets from different AN to estimate the position
    			// In the future we could join all the Broadcasts in one full phase and send them together, here
    			// we insert a transmission in the queue for every received broadcast packet.
				(static_cast<ApplPkt*>(msg))->setDestAddr((static_cast<ApplPkt*>(msg))->getRealDestAddr());
				(static_cast<ApplPkt*>(msg))->setSrcAddr(getParentModule()->findSubmodule("nic"));
				(static_cast<ApplPkt*>(msg))->setRetransmisionCounter(0); // Reset the retransmission counter
				EV << "Enqueing  packet" << endl;
				packetsQueue.insert(msg);
    		} else { // Anchor or computer
    			EV << "Here we should not receive any Broadcast from an AN or the computer" << endl;
    			numOfPcktsRcvdInWrongPhase++;
    			delete msg;
    			msg = 0;
    		}
			break;
		case AppLayer::COM_SINK_PHASE_1:
		case AppLayer::COM_SINK_PHASE_2:
			EV << "In this phase we should not receive any Broadcast, just Reports" << endl;
			numOfPcktsRcvdInWrongPhase++;
			delete msg;
			msg = 0;
			break;
		}
		break;
    default:
    	delete msg;
    	msg = 0;
    }
}


void AnchorAppLayer::handleLowerControl(cMessage *msg)
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
				// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case AppLayer::SYNC_PHASE_1: // In Broadcast case in an Anchor, any of the 3 Sync Phases
				case AppLayer::SYNC_PHASE_2:
				case AppLayer::SYNC_PHASE_3:
					sendDown(msg);
					break;
				default: // If we are in any of the other phases, we don't retransmit
					nbPacketDroppedNoTimeAN++;
					delete msg;
					msg = 0;
					break;
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
				// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case AppLayer::REPORT_PHASE: // In Report case in an Anchor, it can be any phase Report or Com Sink, can be a request answer or a AN to AN communication
				case AppLayer::VIP_PHASE:
				case AppLayer::COM_SINK_PHASE_1:
				case AppLayer::COM_SINK_PHASE_2:
					sendDown(msg);
					break;
				default: // If we are in any of the other phases, we don't retransmit
					nbPacketDroppedNoTimeAN++;
					delete msg;
					msg = 0;
					break;
				}
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
				// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case AppLayer::SYNC_PHASE_1: // In Broadcast case in an Anchor, any of the 3 Sync Phases
				case AppLayer::SYNC_PHASE_2:
				case AppLayer::SYNC_PHASE_3:
					sendDown(msg);
					break;
				default: // If we are in any of the other phases, we don't retransmit
					nbPacketDroppedNoTimeAN++;
					delete msg;
					msg = 0;
					break;
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
				// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case AppLayer::REPORT_PHASE: // In Report case in an Anchor, it can be any phase Report or Com Sink, can be a request answer or a AN to AN communication
				case AppLayer::VIP_PHASE:
				case AppLayer::COM_SINK_PHASE_1:
				case AppLayer::COM_SINK_PHASE_2:
					sendDown(msg);
					break;
				default: // If we are in any of the other phases, we don't retransmit
					nbPacketDroppedNoTimeAN++;
					delete msg;
					msg = 0;
					break;
				}
			}
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			delete msg;
			msg = 0;
		}
		EV << endl;
		break;
	case BaseMacLayer::QUEUE_FULL:
		EV << "The queue in MAC is full, discarding the message" << endl;
		nbPacketDroppedReportAN++;
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

void AnchorAppLayer::sendBroadcast()
{
	// It doesn't matter if we have slotted version or not, CSMA must be disabled, we control random time and retransmission in Appl layer
	ApplPkt *pkt = new ApplPkt("SYNC_MESSAGE_WITHOUT_CSMA", SYNC_MESSAGE_WITHOUT_CSMA);
	pkt->setBitLength(packetLength);

	pkt->setSrcAddr(myNetwAddr);
	pkt->setDestAddr(destination);

	sendDown(pkt);
}
