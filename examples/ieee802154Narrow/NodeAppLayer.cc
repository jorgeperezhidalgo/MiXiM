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
		requestPacketSent = false;
		askFrequencyCounter = 0;
		waitForAnchor = 0;
		waitForRequestTime = 0.02; 			// 20 ms
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
	} else if (stage == 1) {
		// Assign the type of host to 2 (mobile node)
		node = cc->findNic(myNetwAddr);
		node->moduleType = 2;
	} else if (stage == 4) {
		// Add the offset to the first active full phase
		if (offsetPhases > 0) {
			scheduleAt(simTime() + offsetPhases * fullPhaseTime, configureFullPhase);
		} else {
			scheduleAt(simTime(), configureFullPhase);
		}

		// Add the offset to the first extra report full phase, smallTime makes this process start a little bit after the Configure full phase to have all variables already set
		if (offsetReportPhases > 0) {
			scheduleAt(simTime() + offsetReportPhases * fullPhaseTime + smallTime, configureExtraReport);
		} else {
			scheduleAt(simTime() + smallTime, configureExtraReport);
		}
		//BORRAR
		PUTEAR = new cMessage("PUTEAR", SEND_SYNC_TIMER_WITHOUT_CSMA);
//		scheduleAt(2 * timeSyncPhase + timeReportPhase + timeVIPPhase, PUTEAR);
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
	cancelAndDelete(beginPhases);
	//BORRAR
	cancelAndDelete(PUTEAR);
	for(cQueue::Iterator iter(transfersQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
}


void NodeAppLayer::finish()
{
	recordScalar("Dropped Packets in MN - No ACK received", nbPacketDroppedNoACK);
	recordScalar("Dropped Packets in MN - Max MAC BackOff tries", nbPacketDroppedBackOff);
	recordScalar("Dropped Packets in MN - App Queue Full", nbPacketDroppedAppQueueFull);
	recordScalar("Dropped Packets in MN - Mac Queue Full", nbPacketDroppedMacQueueFull);
	recordScalar("Dropped Packets in MN - No Time in the Phase", nbPacketDroppedNoTimeApp);
	recordScalar("Erased Packets in MN - No more BackOff retries", nbErasedPacketsBackOffMax);
	recordScalar("Erased Packets in MN - No more No ACK retries", nbErasedPacketsNoACKMax);
	recordScalar("Erased Packets in MN - No more MAC Queue Full retries", nbErasedPacketsMacQueueFull);
	recordScalar("Number of MN Broadcasts Successfully sent", nbBroadcastPacketsSent);
	recordScalar("Number of MN Reports with ACK", nbReportsWithACK);
	recordScalar("Number of Broadcasts received in MN", nbBroadcastPacketsReceived);
	recordScalar("Number of Reports received in MN", nbReportsReceived);
	recordScalar("Number of Reports really for me received in MN", nbReportsForMeReceived);
	recordScalar("Number of Answers for Requests out of waiting time", nbAnswersRequestOutOfTime);
	recordScalar("Number of Requests without answer from AN", nbRequestWihoutAnswer);
}

void NodeAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case NodeAppLayer::CONFIGURE_FULL_PHASE:
		if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) { // The Type 3 (VIP) doesn't read the sync packets, the rest of the types do
			if (activePhases == 0) {
				EV << "This node has no active phases, the only behaviour is through extra reports" << endl;
			} else if (activePhases > activePhasesCounter) // If we are in an active phase, but not the last one
			{
				activePhase = true;
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
					} else {
						syncListen = true; // Activate to listen
					}
					break;
				case NodeAppLayer::SYNC_PHASE_2:
					scheduleAt(simTime() + timeSyncPhase + timeComSinkPhase, configureFullPhase);
					if (activePhasesCounter == 1 && offsetSyncPhases >= 2) {
						syncListen = false; // Don't listen this sync phase
					} else {
						syncListen = true; // Activate to listen
					}
					break;
				case NodeAppLayer::SYNC_PHASE_3:
					syncListen = true; // Activate to listen
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
				if (activePhases == 0) {
					EV << "This node has no active phases, the only behaviour is through extra reports" << endl;
				} else if (activePhases >= activePhasesCounter) { // All the active phases in MN type 3
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
			scheduleAt(simTime() + timeSyncPhase - smallTime, configureExtraReport);
			if (makeExtraReport) {
				// Check if we are already reading RSSI or not, in case we are, we don't erase the old ones. It is also useful
				// to know if we have to schedule an extra report or if we have already the normal one and we don't have to schedule
				// a new one
				if (!syncListen) { // Any node configuration in an inactive phase
					// Before we start reading RSSI again, we have to reset the previous values to get the new ones
					listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					// Schedule of the Extra Report transmission, note that we leave a guard band at the end
					type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
					scheduleAt(simTime() - smallTime + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
				} else if ((nodeConfig == NodeAppLayer::LISTEN_AND_REPORT || nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (activePhases == activePhasesCounter)) {
					// Do not schedule another extra report, we have already one in this full phase
				} else { // In node configuration type 3 and 4 or in 1 and 2 but not the last active full phase
					// Schedule of the Extra Report transmission, note that we leave a guard band at the end
					type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
					scheduleAt(simTime() - smallTime + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
				}
				// Here we activate the ASK Flag every askFrequency extra reports
				askFrequencyCounter ++;
				if (askFrequencyCounter == askFrequency) {
					askFrequencyCounter = 0;
					askForRequest = true; // Mark this flag for the report transmission that is already scheduled
					// Schedule the Request for Information in the next full phase (period) at the beginning of Report Phase
					scheduleAt(simTime() + fullPhaseTime + timeSyncPhase, waitForRequest);
				} else if (askFrequencyCounter > askFrequency) {
					askFrequencyCounter = 0; // We just put the counter to 0 but don't send any ask, this case happens when askFrequency = 0
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
				numberOfBroadcastsCounter = 0;
			}
			syncListenReport = false;
			if (reportPhases == 0) {
				error ("The reportPhases variable must be bigger than 0");
			} else {
				scheduleAt(simTime() + reportPhases * fullPhaseTime - timeSyncPhase + smallTime, configureExtraReport);
			}
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
				EV << "No need to schedule an extra report as this phase has already one report or extra report." << endl;
			} else {
				EV << "Scheduling extra report for the Request from node addr " << myNetwAddr << " to anchor addr " << anchorDestinationRequest << endl;
				scheduleAt(simTime() + uniform(0, timeReportPhase - guardTimeReportPhase, 0), sendExtraReportWithCSMA);
			}
		} else {
			EV << "Waiting time from an answer from Anchor addr " << anchorDestinationRequest << " finished." <<endl;
			waitForAnchor = 0;
			anchorDestinationRequest = 0;
			nbRequestWihoutAnswer++;
		}
		break;
	case NodeAppLayer::SEND_SYNC_TIMER_WITHOUT_CSMA:
//		// BORRAR
//		if (getParentModule()->getIndex() == 1) { // Solo si es el nodo movil 1
//			ApplPkt *pkt = new ApplPkt("PUTEANDO", SYNC_MESSAGE_WITHOUT_CSMA);
//			pkt->setBitLength(125000);
//			pkt->setDestAddr(destination);
//			pkt->setSrcAddr(myNetwAddr);
//			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
//			sendDown(pkt);
//		}
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
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
	}
}



void NodeAppLayer::handleLowerMsg(cMessage *msg)
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
		// Then filter according to the message type that we receive
		switch (pkt->getKind())
		{
		case NodeAppLayer::REPORT_WITHOUT_CSMA:
		case NodeAppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Mobile Node cannot receive any Report from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Sync Phase Mobile Node cannot receive any Report from a Anchor" << endl;
	    	}
			delete msg;
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Mobile Node cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
				if (syncListen || syncListenReport) { // If I have enabled the listen mode
					nbBroadcastPacketsReceived++;
					EV << "Getting the RSSI from the packet and storing it." << endl;
					// Get the index of the host who sent the packet, it will be the index of the vector to store the RSSI
					indiceRSSI = simulation.getModule(pkt->getSrcAddr())->getParentModule()->getIndex();

					// Initialize the vector to the first value and then add all the rest values, we will divide with the total RSSI read at the end (report or calculation) to get the mean
					if (listRSSI[indiceRSSI].RSSIAdition == 0) {
						listRSSI[indiceRSSI].RSSIAdition = cInfo->getRSSI();
					} else {
						listRSSI[indiceRSSI].RSSIAdition = listRSSI[indiceRSSI].RSSIAdition + cInfo->getRSSI();
					}
					listRSSI[indiceRSSI].counterRSSI ++;
					EV << "Total RSSI received: " << listRSSI[indiceRSSI].RSSIAdition << ", total measured: " << listRSSI[indiceRSSI].counterRSSI << endl;
				} else { // If listening is disabled
					EV << "Discarding the packet, this Sync Phase for the Mobile Node is not activated" << endl;
					getParentModule()->bubble("Don't listen packets!");
				}
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
		if (host->moduleType == 2) { // Mobile Node
			EV << "Discarding the packet, the Mobile Node does not receive any report from another Mobile Node in Report or VIP Phases" << endl;
		} else { // Anchor or Computer
			nbReportsReceived++;
			if (pkt->getDestAddr() == myNetwAddr) { // The packet is for me
				nbReportsForMeReceived++;
				if (waitForAnchor != 0) { // The node received the packet during the waiting time after making a request
					if (pkt->getSrcAddr() == waitForAnchor) { // In wait for anchor we have the Netw Addr from the AN we are waiting the packet from
						// --------------------------------------------------------------------------------------------
						// - Here we would get info from the AN and we have to program here what to do with this info -
						// --------------------------------------------------------------------------------------------
						getParentModule()->bubble("I've received the message");
						cancelEvent(waitForRequest); // We have to cancel the waiting event as we already received the info
						waitForAnchor = 0;
						anchorDestinationRequest = 0;
					} else {
						EV << "Waiting time assigned to another anchor, received packet from wrong anchor" << endl;
					}
				} else { // The node received the packet out of the waiting time after making a request
					EV << "Received packet out of the waiting time" << endl;
					nbAnswersRequestOutOfTime++;
				}
			} else {
				EV << "Discard the packet, the Mobile Node doesn't route the packet" << endl;
			}
		}
		delete msg;
		break;
	case AppLayer::COM_SINK_PHASE_1:
	case AppLayer::COM_SINK_PHASE_2:
		EV << "Discarding packet, the Mobile Node doesn't receive any packet during Com Sink Phases" << endl;
		delete msg;
		break;
	default:
		EV << "Unkown Phase! -> deleting packet, kind: " << msg->getKind() << endl;
		delete msg;
	}
	delete cInfo;
}

void NodeAppLayer::handleLowerControl(cMessage *msg)
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
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::REPORT_PHASE: // In Mobile Node case, it only transmits on the Report or VIP phases
			case AppLayer::VIP_PHASE:
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
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedNoACK++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if (pkt->getRetransmisionCounterACK() < maxRetransDroppedReportMN) {
			pkt->setRetransmisionCounterACK(pkt->getRetransmisionCounterACK() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterACK() << " of " << maxRetransDroppedReportMN;
			// In case the packet transmission failed, we have to check before retransmission that we are still in the transmission phase
			switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
			{
			case AppLayer::REPORT_PHASE: // In Mobile Node case, it only transmits on the Report or VIP phases
			case AppLayer::VIP_PHASE:
				transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
				sendDown(pkt);
				break;
			default: // If we are in any of the other phases, we don't retransmit
				nbPacketDroppedNoTimeApp++;
				delete pkt;
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
		// In case we have transmitted a request, we have to start the receiving timer when we received the ACK and consequently the SUCCESS in App
		if (pkt->getRequestPacket()) {
			waitForAnchor = pkt->getDestAddr(); // Assign the variable the MAC addr of the destination AN, to detect when a packet comes from this and cancel the timing
			scheduleAt(simTime() + waitForRequestTime, waitForRequest);
			EV << "The Packet was a Request, so it starts Waiting for Request Timer (" << waitForRequestTime << "s), to wait for Request answer from AN addr " << pkt->getDestAddr() << endl;
		}
		delete pkt;
		break;
	}
	delete msg;
}

void NodeAppLayer::sendBroadcast()
{
	ApplPkt *pkt = new ApplPkt("Broadcast Message with CSMA", SYNC_MESSAGE_WITH_CSMA);

	pkt->setNumberOfBroadcasts(numberOfBroadcasts);
	pkt->setBitLength(packetLength);
	pkt->setSrcAddr(myNetwAddr);
	pkt->setRealSrcAddr(myNetwAddr);
	pkt->setDestAddr(destination);
	pkt->setRealDestAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic"));
	pkt->setCSMA(true);

	transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
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

	// We assign here the Destination Address for the next hop
    if (selectedAnchor == numberOfAnchors) { // The Computer (this case is not possible as the computer doesn't send sync packets)
    	netwAddr = getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic");
    } else if (selectedAnchor >= 0 && selectedAnchor <numberOfAnchors) { // The Anchors
    	netwAddr = getParentModule()->getParentModule()->getSubmodule("anchor",selectedAnchor)->findSubmodule("nic");
    } else {
    	EV << "No RSSI was read, so no possible to select an Anchor" << endl;
    	netwAddr = 0;
    }
    pkt->setDestAddr(netwAddr);

    // We assign here the Real Destination Address, for the last destination
	if (centralized && !requestPacket) {
		// If the Node Configuration is in centralized, all reports must go to the computer but only if this is not a request packet
		pkt->setRealDestAddr(getParentModule()->getParentModule()->getSubmodule("computer",0)->findSubmodule("nic"));
	} else { // If its Distributed-A the real destination of the report is the selected Anchor, also if it is a request packet
		pkt->setRealDestAddr(netwAddr);
	}

	if ((netwAddr != 0) || (anchorDestinationRequest != 0)) { // If we have a destination address, now meassured or from the previous ask report
        pkt->setBitLength(packetLength);
		pkt->setSrcAddr(myNetwAddr);
		pkt->setRealSrcAddr(myNetwAddr);
		pkt->setCSMA(true);

		// FLAG management
		if (askForRequest) {
			pkt->setAskForRequest(true); // Set Ask Flag = true
			anchorDestinationRequest = netwAddr; // Copies the selected Anchor Netw Addr to have it available in the request report
			EV << "Selected Anchor for ASK Report with MAC Addr: " << anchorDestinationRequest << endl;
			requestPacket = true;
			askForRequest = false;
		} else if (requestPacket) {
			pkt->setRequestPacket(true); // Set Request Flag = true
			pkt->setDestAddr(anchorDestinationRequest); // Redefine Dest Addr
			pkt->setRealDestAddr(anchorDestinationRequest); // Redefine Real Dest Addr
			EV << "Selected Anchor for Request Report with MAC Addr: " << anchorDestinationRequest << endl;
			requestPacket = false;
		}

		EV << "Send Report to Anchor with Netw Addr " << pkt->getDestAddr() << ", with end destination Netw Addr " << pkt->getRealDestAddr() << endl;

		// Before we transmit the Report, we have to check if we are still in Report phase
		switch(InWhichPhaseAmI(fullPhaseTime, timeSyncPhase, timeReportPhase, timeVIPPhase, timeComSinkPhase))
		{
		case AppLayer::REPORT_PHASE: // Any Report from any Mobile Node type, will be transmitted in Report Phase
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
			break;
		default: // If we are in any of the other phases, we don't transmit the report
			nbPacketDroppedNoTimeApp++;
			delete pkt;
		}
    } else { // We didn't have any RSSI value or old selected Anchor
    	EV << "Report not sent as there was no RSSI values to obtain the selected Anchor" << endl;
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
