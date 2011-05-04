#include "NodeAppLayer.h"

Define_Module(NodeAppLayer);

void NodeAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if(stage == 0) {
		// Parameter load
		nodeConfig = par("nodeConfig");
		offsetPhases = par("offsetPhases");
		offsetSyncPhases = par("offsetSyncPhases");
		activePhases = par("activePhases");
		inactivePhases = par("inactivePhases");
		offsetReportPhases = par("offsetReportPhases");
		reportPhases = par("reportPhases");
		makeExtraReport = par("makeExtraReport");
		askFrequency = par("askFrequency");
		numberOfBroadcasts = par("NumberOfBroadcasts");
		positionsToSave = par("positionsToSave");
		centralized = par("centralized");

		// Variable initialization, we could change this into parameters if necesary
		activePhasesCounter = 1;
		syncListen = false;
		syncListenReport = false;
		numberOfBroadcastsCounter = 0;
		minimumSeparation = 0.002; 			// Separation time 2 ms for example, redefine if we want other
		minimumDistanceFulfilled = false;
		positionsSavedCounter = 0;
		processingTime = 0.000870; 			// 870 ns MinMax Algorithm
		isProcessing = false;
		askForRequest = false;
		requestPacket = false;
		askFrequencyCounter = 0;
		waitForAnchor = 0;
		waitForRequestTime = 0.02; 			// 20 ms
		nbPacketRequestedFailed = 0;
		anchorDestinationRequest = 0;

		// Start all the arrays to 0 and with the appropriate size reserved in memory
		listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
		randomTransTimes = (simtime_t*)calloc(sizeof(simtime_t), numberOfBroadcasts);
		positionFIFO = (Coord*)calloc(sizeof(Coord), positionsToSave);

		// Creation of the messages that will be scheduled to send packets of configure the node
		configureFullPhase = new cMessage("configuring full phase", CONFIGURE_FULL_PHASE);
		configureExtraReport = new cMessage("configuring extra report", CONFIGURE_EXTRA_REPORT);
		sendReportWithCSMA = new cMessage("send report with CSMA", SEND_REPORT_WITH_CSMA);
		sendExtraReportWithCSMA = new cMessage("send extra report with CSMA", SEND_REPORT_WITH_CSMA);
		sendSyncTimerWithCSMA = new cMessage("send sync timer with CSMA", SEND_SYNC_TIMER_WITH_CSMA);
		calculatePosition = new cMessage("calculate position", CALCULATE_POSITION);
		waitForRequest = new cMessage("waiting for request",WAITING_REQUEST);

		//BORRAR
		PUTEAR = new cMessage("PUTEAR", SEND_REPORT_WITHOUT_CSMA);
		scheduleAt(1.0, PUTEAR);

	} else if (stage == 1) {
		// Assign the type of host to 2 (mobile node)
		node = cc->findNic(getParentModule()->findSubmodule("nic"));
		node->moduleType = 2;
	} else if (stage == 4) {
		// Add the offset to the first active full phase
		if (offsetPhases > 0) {
			scheduleAt(simTime() + offsetPhases * fullPhaseTime, configureFullPhase);
		} else {
			scheduleAt(simTime(), configureFullPhase);
		}

		// Add the offset to the first extra report full phase
		if (offsetReportPhases > 0) {
			scheduleAt(simTime() + offsetReportPhases * fullPhaseTime, configureExtraReport);
		} else {
			scheduleAt(simTime(), configureExtraReport);
		}
	}
}

NodeAppLayer::~NodeAppLayer() {
	cancelAndDelete(configureFullPhase);
	cancelAndDelete(configureExtraReport);
	cancelAndDelete(sendReportWithCSMA);
	cancelAndDelete(sendExtraReportWithCSMA);
	cancelAndDelete(sendSyncTimerWithCSMA);
	cancelAndDelete(calculatePosition);
	cancelAndDelete(waitForRequest);
	//BORRAR
	cancelAndDelete(PUTEAR);
}


void NodeAppLayer::finish()
{
	recordScalar("dropped_MN", nbPacketDroppedReportMN);
	recordScalar("dropped_backoff_MN", nbPacketDroppedBackOffMN);
	recordScalar("failed_requests", nbPacketRequestedFailed);
	recordScalar("dropped_no_time_in_MN", nbPacketDroppedNoTimeMN);
	recordScalar("packets_rcv_in_wrong_phase", numOfPcktsRcvdInWrongPhase);
}

void NodeAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case NodeAppLayer::CONFIGURE_FULL_PHASE:
		if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) { // The Type 3 (VIP) doesn't read the sync packets, the rest of the types do
			if (activePhases > activePhasesCounter) // If we are in an active phase, but not the last one
			{
				activePhase = true;
				syncListen = true; // Activate to listen
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case NodeAppLayer::SYNC_PHASE_1:
					scheduleAt(simTime() + timeSyncPhase + timeReportPhase + timeVIPPhase, configureFullPhase); // Schedule this event for the beggining of the next sync phase
					if (activePhasesCounter == 1) { // If we are in the first active phase
						// Before we start reading RSSI again, we have to reset the previous values to get the new ones
						listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					}
					if (nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) { // I node is type 4
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
						// Schedule of the First Random Broadcast transmission
						scheduleAt(simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter], sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						numberOfBroadcastsCounter ++;
					}
					if (activePhasesCounter == 1 && offsetSyncPhases >= 1) {
						syncListen = false; // Don't listen this sync phase
					}
					break;
				case NodeAppLayer::SYNC_PHASE_2:
					scheduleAt(simTime() + timeSyncPhase + timeComSinkPhase, configureFullPhase);
					if (activePhasesCounter == 1 && offsetSyncPhases >= 2) {
						syncListen = false; // Don't listen this sync phase
					}
					break;
				case NodeAppLayer::SYNC_PHASE_3:
					activePhasesCounter ++;
					scheduleAt(simTime() + timeSyncPhase + timeComSinkPhase, configureFullPhase);
					// Here is no sense disabling listening like in the previous two phases, because if we disable 3 phases, it's like reducing an active phase
					break;
				default:
					error("How can you be in other phase?");
				}
			}
			else if (activePhases == activePhasesCounter) // When the last active phase or the first one if there is only one
			{
				switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
				{
				case NodeAppLayer::SYNC_PHASE_1:
					activePhase = true;
					syncListen = true;
					scheduleAt(simTime() + timeSyncPhase + timeReportPhase + timeVIPPhase, configureFullPhase);
					if (activePhases == 1) { // If there is only one active phase, this "else" will execute and not the "if" before
						// Before we start reading RSSI again, we have to reset the previous values to get the new ones
						listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					}
					if (nodeConfig == NodeAppLayer::LISTEN_AND_REPORT) {
						// Schedule of the Report transmission, note that we leave a guard band at the end
						scheduleAt(simTime() + timeSyncPhase + uniform(0, timeReportPhase - guardTimeReportPhase, 0), sendReportWithCSMA);
					} else if (nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) {
						// Schedule of the Report transmission, note that we leave a guard band at the end
						type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
						scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendReportWithCSMA);
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						// We have to leave also this minimum separation with the report just generated in type4ReportTime
						createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
						// Schedule of the First Random Broadcast transmission
						scheduleAt(simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter], sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						numberOfBroadcastsCounter ++;
					} else { // Must be the Type 2 because type 3 doesn't come into the big if
						// Makes an event to calculate the position at start from next Report phase
						scheduleAt(simTime() + timeSyncPhase, calculatePosition);
					}
					break;
				case NodeAppLayer::SYNC_PHASE_2:
					syncListen = false;
					scheduleAt(simTime() + timeSyncPhase + timeComSinkPhase, configureFullPhase);
					break;
				case NodeAppLayer::SYNC_PHASE_3:
					syncListen = false;
					activePhasesCounter ++;
					scheduleAt(simTime() + timeSyncPhase + timeComSinkPhase, configureFullPhase);
					break;
				default:
					error("How can you be in other phase?");
				}
			}
			else
			{
				activePhase = false;
				syncListen = false;
				activePhasesCounter = 1;
				scheduleAt(simTime() + (inactivePhases * fullPhaseTime), configureFullPhase);
			}
		} else { // The Type 3 (VIP), has to send reports in Broadcast mode all the active periods
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case NodeAppLayer::SYNC_PHASE_1:
				syncListen = false;
				if (activePhases >= activePhasesCounter) { // All the active phases in MN type 3
					activePhase = true;
					scheduleAt(simTime() + fullPhaseTime, configureFullPhase); // Program an event on start of next active phase
					// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
					createRandomBroadcastTimes(timeVIPPhase - guardTimeVIPPhase);
					// Schedule of the First Random Broadcast transmission
					scheduleAt(simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[numberOfBroadcastsCounter], sendSyncTimerWithCSMA); // Program an event to send the broadcasts
					numberOfBroadcastsCounter ++;
					activePhasesCounter ++;
				} else { // All the inactive phases in MN type 3
					activePhase = false;
					scheduleAt(simTime() + (inactivePhases * fullPhaseTime), configureFullPhase); // Program an event on start of next active phase
					activePhasesCounter = 1;
				}
				break;
			default:
				error("Mobile Node type 3 can't have an event of this type out of Sync Phase 1");
			}
		}
		break;
	case NodeAppLayer::CONFIGURE_EXTRA_REPORT:
		if (!syncListenReport) {
			syncListenReport = true; // To listen to the 1st sync phase to take the RSSI measurements to know your selected AN
			scheduleAt(simTime() + timeSyncPhase, configureExtraReport);
			if (makeExtraReport) {
				// Check if we are already reading RSSI or not, in case we are, we don't erase the old ones. It is also useful
				// to know if we have to schedule an extra report or if we have already the normal one and we don't have to schedule
				// a new one
				if (!syncListen) { // Any node configuration in an inactive phase
					// Before we start reading RSSI again, we have to reset the previous values to get the new ones
					listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					// Schedule of the Extra Report transmission, note that we leave a guard band at the end
					type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
					scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
				} else if ((nodeConfig == NodeAppLayer::LISTEN_AND_REPORT || nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (activePhases == activePhasesCounter)) {
					// Do not schedule another extra report, we have already one in this full phase
				} else { // In node configuration type 3 and 4 or in 1 and 2 but not the last active full phase
					// Schedule of the Extra Report transmission, note that we leave a guard band at the end
					type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
					scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
				}
				// Here we activate the ASK Flag every askFrequency extra reports
				askFrequencyCounter ++;
				if (askFrequencyCounter == askFrequency) {
					askFrequencyCounter = 0;
					askForRequest = true; // Mark this flag for the report transmission that is already scheduled
					// Schedule the Request for Information in the next full phase (period) at the beginning of Report Phase
					scheduleAt(simTime() + fullPhaseTime + timeSyncPhase, waitForRequest);
				}
			}
		} else {
			if ((nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && syncListen && makeExtraReport) { // If MN type = 4 in active report and we have to make an extra report in this phase
				// Recalculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
				// We have to do this to take into account for the minimum separation the extra report just generated in type4ReportTime
				createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
				// Cancel the scheduled First Random Broadcast transmission and schedule it again with the new time
				cancelEvent(sendSyncTimerWithCSMA);
				numberOfBroadcastsCounter = 0;
				scheduleAt(simTime() + randomTransTimes[numberOfBroadcastsCounter], sendSyncTimerWithCSMA); // Program an event to send the broadcasts
				numberOfBroadcastsCounter ++;
			}
			if (nodeConfig == NodeAppLayer::VIP_TRANSMIT && makeExtraReport && activePhase) {
				// If we schedule an extra report, with the type 3, we cancel the broadcasts in this full phase (period) to save battery
				cancelEvent(sendSyncTimerWithCSMA);
			}
			syncListenReport = false;
			scheduleAt(simTime() + reportPhases * fullPhaseTime - timeSyncPhase, configureExtraReport);
		}
		break;
	case NodeAppLayer::SEND_REPORT_WITH_CSMA:
		if (!isProcessing) // If are still calculation the position in Mobile Node type 2
			sendReport();
		else
			scheduleAt(endProcessingTime + uniform(0, timeReportPhase - processingTime - guardTimeReportPhase, 0), sendExtraReportWithCSMA);
		break;
	case NodeAppLayer::SEND_SYNC_TIMER_WITH_CSMA:
		sendBroadcast();
		if (numberOfBroadcastsCounter < numberOfBroadcasts) {
			// Schedule the next random broadcast, we have to subtract first the previous random time to be at the begining of the phase
			// and then add the new random time (relative to the start of VIP phase or Report Phase for type 3 and 4)
			scheduleAt(simTime() - randomTransTimes[numberOfBroadcastsCounter - 1] + randomTransTimes[numberOfBroadcastsCounter], sendSyncTimerWithCSMA);
			numberOfBroadcastsCounter ++;
		} else {
			numberOfBroadcastsCounter = 0;
		}
		break;
	case NodeAppLayer::CALCULATE_POSITION:
		// Here we would calculate the position and store it in the FIFO, now we just save our real position
		if (!isProcessing) {
			insertElementInFIFO(node->pos);
			isProcessing = true;
			endProcessingTime = simTime() + processingTime;
			scheduleAt(endProcessingTime, calculatePosition);
		} else {
			isProcessing = false;
		}
		break;
	case NodeAppLayer::WAITING_REQUEST:
		if (waitForAnchor == 0) {
			// The Report packet sent the full phase before can only be an extra report, as the ask is activated every number of extra reports
			// So in this full phase (period) this report can only be at the same time as a normal report, we have to check this to send only one report and not 2
			// It is no possible then having extra reports everyphase, and it's also no sense
			if ((nodeConfig == NodeAppLayer::LISTEN_AND_REPORT || nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (activePhases == activePhasesCounter) && syncListen) {
				// We don't need to schedule a new Report send for the Request, we have already the standard Report
			} else {
				scheduleAt(simTime() + uniform(0, timeReportPhase - guardTimeReportPhase, 0), sendExtraReportWithCSMA);
			}
		} else {
			waitForAnchor = 0;
			nbPacketRequestedFailed ++;
		}
		break;
	case NodeAppLayer::SEND_REPORT_WITHOUT_CSMA:
//		// BORRAR
//		if (getParentModule()->getIndex() == 1) { // Solo si es el nodo movil 1
//			ApplPkt *pkt = new ApplPkt("PUTEANDO", REPORT_WITHOUT_CSMA);
//			pkt->setBitLength(80000);
//			pkt->setDestAddr(destination);
//			pkt->setSrcAddr(getParentModule()->findSubmodule("nic"));
//			sendDown(pkt);
//		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
	}
}



void NodeAppLayer::handleLowerMsg(cMessage *msg)
{
	// We could here filter also with the fase we are in, but we assume we are only receiving Broadcasts in sync phases and reports in Report or VIP phases
	switch( msg->getKind() )
    {
    case NodeAppLayer::REPORT_WITHOUT_CSMA:
    case NodeAppLayer::REPORT_WITH_CSMA:
    	if ((static_cast<ApplPkt*>(msg))->getDestAddr() == getParentModule()->findSubmodule("nic")) {
    		if (waitForAnchor != 0) {
				if (((static_cast<ApplPkt*>(msg))->getSrcAddr() == waitForAnchor)) {
					// Here we would get info from the AN and we have to program here what to do with this info
					getParentModule()->bubble("I've received the message");
					cancelEvent(waitForRequest);
					waitForAnchor = 0;
				} else {
					EV << "I got a message in the correct time but from the wrong Anchor." << endl;
				}
    		} else {
    			nbRqstWihoutAnswer++;
    			EV << "Received packet out of receiving time." << endl;
    		}
    	} else { // Do nothing, the Mobile Node doesn't route the packet
    	}
		delete msg;
		msg = 0;
    	break;
    case NodeAppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
    case NodeAppLayer::SYNC_MESSAGE_WITH_CSMA:
    	if (syncListen || syncListenReport) {
    		assert(dynamic_cast<NetwControlInfo*>(msg->getControlInfo()));
    		NetwControlInfo* cInfo = static_cast<NetwControlInfo*>(msg->getControlInfo());

    		// Get the index of the host who sent the packet, it will be the index of the vector to store the RSSI
    		host = cc->findNic(simulation.getModule((static_cast<ApplPkt*>(msg))->getSrcAddr())->getParentModule()->findSubmodule("nic"));
    		if (host->moduleType == 3)
    			indiceRSSI = numberOfAnchors;
    		else
    			indiceRSSI = simulation.getModule((static_cast<ApplPkt*>(msg))->getSrcAddr())->getParentModule()->getIndex();

    		EV << "La RSSI en Appl es: " << cInfo->getRSSI() << " desde el AN: " << indiceRSSI << endl;
    		EV << "La BER en Appl es: " << cInfo->getBitErrorRate() << endl;

    		// Initializate the vector to the first value and then add all the rest values, we will divide with the total RSSI read at the end (report or calculation) to get the mean
    		if (listRSSI[indiceRSSI].RSSIAdition == 0) {
    			listRSSI[indiceRSSI].RSSIAdition = cInfo->getRSSI();
    		} else {
    			listRSSI[indiceRSSI].RSSIAdition = listRSSI[indiceRSSI].RSSIAdition + cInfo->getRSSI();
    		}
    		listRSSI[indiceRSSI].counterRSSI ++;
    		EV << "Total RSSI: " << listRSSI[indiceRSSI].RSSIAdition << ", total measured: " << listRSSI[indiceRSSI].counterRSSI << endl;
    	} else {
    		getParentModule()->bubble("Don't listen packets!");
    	}
    	delete msg;
    	msg = 0;
    	break;
    default:
    	delete msg;
    	msg = 0;
    }
}

void NodeAppLayer::handleLowerControl(cMessage *msg)
{
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		nbPacketDroppedBackOffMN++;
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
				// In case we are transmitting a request, we have to restart the receiving timer again
				if ((static_cast<ApplPkt*>(msg))->getRequestPacket()) {
					requestPacket = true;
					cancelAndDelete(waitForRequest); // It'll be rescheduled in the Send Report method
				}
			}
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::REPORT_PHASE: // In Mobile Node case, it only transmits on the Report or VIP phases
			case AppLayer::VIP_PHASE:
				sendDown(msg);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeMN++;
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
		nbPacketDroppedReportMN++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if ((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() < maxRetransDroppedReportMN) {
			(static_cast<ApplPkt*>(msg))->setRetransmisionCounter((static_cast<ApplPkt*>(msg))->getRetransmisionCounter() + 1);
			if ((static_cast<ApplPkt*>(msg))->getDestAddr() == destination) { // Its a broadcast (it cannot be, but just in case)
				EV << " Broadcast retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedReportMN;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITH_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("SYNC_MESSAGE_WITHOUT_CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(SYNC_MESSAGE_WITHOUT_CSMA);
				}
			} else { // Its a Report
				EV << " Report retransmission number " << (static_cast<ApplPkt*>(msg))->getRetransmisionCounter() << " of " << maxRetransDroppedReportMN;
				if ((static_cast<ApplPkt*>(msg))->getCSMA()) { // Set Name and Kind of the packet for CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report with CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITH_CSMA);
				} else { // Set Name and Kind of the packet for non CSMA transmission
					(static_cast<ApplPkt*>(msg))->setName("Report without CSMA");
					(static_cast<ApplPkt*>(msg))->setKind(REPORT_WITHOUT_CSMA);
				}
				// In case we are transmitting a request, we have to restart the receiving timer again
				if ((static_cast<ApplPkt*>(msg))->getRequestPacket()) {
					requestPacket = true;
					cancelAndDelete(waitForRequest); // It'll be rescheduled in the Send Report method
				}
			}
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::REPORT_PHASE: // In Mobile Node case, it only transmits on the Report or VIP phases
			case AppLayer::VIP_PHASE:
				sendDown(msg);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeMN++;
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
		EV << "The queue in MAC is full, discarding the message" << endl;
		nbPacketDroppedReportMN++;
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

void NodeAppLayer::sendBroadcast()
{
	ApplPkt *pkt = new ApplPkt("Broadcast Message with CSMA", SYNC_MESSAGE_WITH_CSMA);

	pkt->setBitLength(packetLength);
	pkt->setSrcAddr(getParentModule()->findSubmodule("nic"));
	pkt->setRealSrcAddr(getParentModule()->findSubmodule("nic"));
	pkt->setDestAddr(destination);
	pkt->setRealDestAddr(arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic")));
	pkt->setCSMA(true);
	sendDown(pkt);
}

void NodeAppLayer::sendReport()
{
	int selectedAnchor = -1;
	double highestRSSI = 0.0;
	int netwAddr;

	ApplPkt *pkt = new ApplPkt("Report with CSMA", REPORT_WITH_CSMA);

	// Print all the RSSI from all the AN and looks for the bigger to get the selected Anchor
	for (int i = 0; i < (numberOfAnchors); i++) {
		EV << "RSSI: " << (listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI) << " en Anchor " << i << endl;
		if ((listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI) > highestRSSI) {
			highestRSSI = listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI;
			selectedAnchor = i;
		}
	}

    if (selectedAnchor == numberOfAnchors) { // The Computer (this case is not possible as the computer doesn't send sync packets)
    	netwAddr = arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic"));
    } else if (selectedAnchor >= 0 && selectedAnchor <numberOfAnchors) { // The Anchors
    	netwAddr = arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("anchor",selectedAnchor)->findSubmodule("nic"));
    } else {
    	EV << "No RSSI was read, so no possible to select an Anchor" << endl;
    	netwAddr = destination;
    }

    EV << "Enviar a: " << selectedAnchor << endl;
    pkt->setBitLength(packetLength);
	pkt->setDestAddr(netwAddr);
	pkt->setSrcAddr(getParentModule()->findSubmodule("nic"));
	if (centralized)
		pkt->setRealDestAddr(arp->getMacAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic")));
	else
		pkt->setRealDestAddr(netwAddr);
	pkt->setRealSrcAddr(getParentModule()->findSubmodule("nic"));
	pkt->setCSMA(true);

	// FLAG management before sending
	if (askForRequest) {
		pkt->setAskForRequest(true);
		anchorDestinationRequest = netwAddr;
		EV << "Selected Anchor for ASK Report with MAC Addr: " << anchorDestinationRequest << endl;
	} else if (requestPacket) {
		pkt->setRequestPacket(true);
		pkt->setDestAddr(anchorDestinationRequest);
		pkt->setRealDestAddr(anchorDestinationRequest);
		waitForAnchor = anchorDestinationRequest; // Assign the variable the MAC addr of the destination AN, to detect when a packet comes from this and cancel the timing
		scheduleAt(simTime() + waitForRequestTime, waitForRequest);
		EV << "Selected Anchor for ASK Request with MAC Addr: " << anchorDestinationRequest << endl;
	}


	// COMPROBAR BIEN QUE ESTAMOS EN UNA FASE DONDE SE PUEDE MANDAR ANTES DE HACERLO, POR SI SE RETRASAN LOS ENVIOS ASEGURARNOS QUE NO PISAMOS NADA
	sendDown(pkt);

	// FLAG management after sending
	if (askForRequest) {
		requestPacket = true;
		askForRequest = false;
	} else if (requestPacket) {
		requestPacket = false;
	}
}

void NodeAppLayer::insertElementInFIFO(Coord position)
{
	if (positionsSavedCounter < positionsToSave) {
		positionFIFO[positionsSavedCounter] = position;
		positionsSavedCounter ++;
	} else {
		for (int i = 0; i < positionsToSave - 1; i++)
		{
			positionFIFO[i] = positionFIFO[i+1];
		}
		positionFIFO[positionsToSave - 1] = position;
	}
}

void NodeAppLayer::insertElementInFIFO(double x, double y)
{
	Coord position;
	position.setX(x);
	position.setY(y);

	if (positionsSavedCounter < positionsToSave) {
		positionFIFO[positionsSavedCounter] = position;
		positionsSavedCounter ++;
	} else {
		for (int i = 0; i < positionsToSave - 1; i++)
		{
			positionFIFO[i] = positionFIFO[i+1];
		}
		positionFIFO[positionsToSave - 1] = position;
	}
}

Coord NodeAppLayer::extractElementFIFO()
{
	Coord result;

	if (positionsSavedCounter == 0) {
		error("There are no elements in the FIFO");
	} else {
		result = positionFIFO[0];
		if (positionsSavedCounter > 1) {
			for (int i = 0; i < positionsSavedCounter - 1; i++)
			{
				positionFIFO[i] = positionFIFO[i+1];
			}
		}
		positionFIFO[positionsSavedCounter - 1] = (Coord*)calloc(sizeof(Coord), 1);
		positionsSavedCounter --;
	}
	return result;
}

void NodeAppLayer::createRandomBroadcastTimes(simtime_t maxTime)
{
	for (int i = 0; i < numberOfBroadcasts; i++)
	{
		while (!minimumDistanceFulfilled)
		{
			minimumDistanceFulfilled = true;
			randomTransTimes[i] = uniform(0, maxTime, 0);
			for (int j = 0; j <= i; j++)
			{
				// Compare times of the previous generated broadcasts with the new one to check minimum distance
				simtime_t diff = randomTransTimes[i] - randomTransTimes[j];
				if (((diff<0 ? -diff : diff) <= minimumSeparation) && i != j)
				{
					minimumDistanceFulfilled = false;
				}
			}
			if ((nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (syncListenReport || (activePhases == activePhasesCounter))) {
			// If MN is type 4 and there is a report or extra report in this phase,
			// check minimum distance with just generated broadcast random time
				simtime_t diff = randomTransTimes[i] - type4ReportTime;
				if ((diff<0 ? -diff : diff) <= minimumSeparation)
				{
					minimumDistanceFulfilled = false;
				}
			}
		}
		minimumDistanceFulfilled = false;
	}
	orderVectorMinToMax(randomTransTimes, numberOfBroadcasts);
}

void NodeAppLayer::orderVectorMinToMax(simtime_t* randomTransTimes, int numberOfBroadcasts)
{
	int i,j;

	for (i = 0; i < numberOfBroadcasts; i++)
	{
		for (j = 0; j < i; j++)
		{
			if (randomTransTimes[i] < randomTransTimes[j])
			{
				simtime_t temp = randomTransTimes[i];
				randomTransTimes[i] = randomTransTimes[j];
				randomTransTimes[j] = temp;
			}
		}
	}
	for (i = 0; i < numberOfBroadcasts; i++)
		EV << "Broadcast random time " << i + 1 << " : " << randomTransTimes[i] << endl;
}
