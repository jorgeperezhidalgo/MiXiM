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
		timeSleepToRX = getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeSleepToRX");
		timeRXToSleep = getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeRXToSleep");

		// Variable initialization, we could change this into parameters if necesary
		syncListen 					= false;
		syncListenReport 			= false;
		numberOfBroadcastsCounter 	= 0;
		sleepTimesCounter			= 0;
		wakeUpTimesCounter			= 0;
		minimumSeparation 			= 0.002;	// Separation time 2 ms for example, redefine if we want other
		minimumDistanceFulfilled 	= false;
		positionsSavedCounter 		= 0;
		processingTime 				= 0.000870; // 870 ns MinMax Algorithm
		isProcessing 				= false;
		askForRequest 				= false;
		requestPacket 				= false;
		askFrequencyCounter 		= 0;
		waitForAnchor 				= 0;
		waitForRequestTime 			= 0.02; 	// 20 ms
		anchorDestinationRequest 	= 0;

		offsetPhasesCounter			= 0;
		activePhasesCounter			= 0;
		inactivePhasesCounter		= inactivePhases; // To start with an active period, with 0 it will be with an inactive period
		offsetReportPhasesCounter	= 0;
		reportPhasesCounter			= reportPhases; // The first period after the offset will be extra report period, if 0 then the last reportPhases period


		// Start all the arrays to 0 and with the appropriate size reserved in memory
		listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
		randomTransTimes = (simtime_t*)calloc(sizeof(simtime_t), numberOfBroadcasts);
		positionFIFO = (Coord*)calloc(sizeof(Coord), positionsToSave);
		listSleepTimes = (simtime_t*)calloc(sizeof(simtime_t), 100); // Maximum of 100 times, more than enough
		listWakeUpTimes = (simtime_t*)calloc(sizeof(simtime_t), 100); // Maximum of 100 times, more than enough
		wakeUpTransmitting = (bool*)calloc(sizeof(bool), 100); // Maximum of 100 times, more than enough


		// Creation of the messages that will be scheduled to send packets of configure the node
		sendReportWithCSMA = new cMessage("send report with CSMA", SEND_REPORT_WITH_CSMA);
		sendExtraReportWithCSMA = new cMessage("send extra report with CSMA", SEND_REPORT_WITH_CSMA);
		sendSyncTimerWithCSMA = new cMessage("send sync timer with CSMA", SEND_SYNC_TIMER_WITH_CSMA);
		calculatePosition = new cMessage("calculate position", CALCULATE_POSITION);
		waitForRequest = new cMessage("waiting for request",WAITING_REQUEST);
		wakeUp = new cMessage("waking up node",WAKE_UP);
		sleep = new cMessage("sleeping the node",SLEEP);


    	// get handler to phy layer
        phy = FindModule<MacToPhyInterface*>::findSubModule(getParentModule());

        // get handler to energy module
    	energy = static_cast<EnergyConsumption*>(getParentModule()->getSubmodule("energy"));
	} else if (stage == 1) {
		// Assign the type of host to 2 (mobile node)
		node = cc->findNic(myNetwAddr);
		node->moduleType = 2;
	} else if (stage == 4) {
		//BORRAR
		PUTEAR = new cMessage("PUTEAR", SEND_SYNC_TIMER_WITHOUT_CSMA);
//		scheduleAt(timeSyncPhase + fullPhaseTime, PUTEAR);
//		goToWakeUp(timeSyncPhase + fullPhaseTime - timeSleepToRX, true);
	}
}

NodeAppLayer::~NodeAppLayer() {
	cancelAndDelete(sendReportWithCSMA);
	cancelAndDelete(sendExtraReportWithCSMA);
	cancelAndDelete(sendSyncTimerWithCSMA);
	cancelAndDelete(calculatePosition);
	cancelAndDelete(waitForRequest);
	cancelAndDelete(beginPhases);
	cancelAndDelete(wakeUp);
	cancelAndDelete(sleep);
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
	case NodeAppLayer::SLEEP:
		EV << "Changing PHY state to Sleep" << endl;
		phy->setRadioState(Radio::SLEEP);
		energy->updateStateStatus(false, EnergyConsumption::NUM_MAC_STATES, Radio::SLEEP);
		// Erase this time element
		sleepTimesCounter--;
		for (int i = 1; i <= sleepTimesCounter; i++) { // Remove the first element (the smaller)
			listSleepTimes[i-1] = listSleepTimes[i];
		}
		// Schedule next time element
		if (sleepTimesCounter > 0) {
			scheduleAt(listSleepTimes[0], sleep);
		}
		break;
	case NodeAppLayer::WAKE_UP:
		if (phy->getRadioState() == Radio::SLEEP) {
			EV << "Changing PHY state to RX" << endl;
			phy->setRadioState(Radio::RX);
			energy->updateStateStatus(wakeUpTransmitting[0], EnergyConsumption::NUM_MAC_STATES, Radio::RX);
		} else {
			EV << "No need to change PHY state to RX, its already woken up" << endl;
//			energy->updateStateStatus(wakeUpTransmitting[0], EnergyConsumption::NUM_MAC_STATES, Radio::RX);
		}
		// Erase this time element
		wakeUpTimesCounter--;
		for (int i = 1; i <= wakeUpTimesCounter; i++) { // Remove the first element (the smaller)
			listWakeUpTimes[i-1] = listWakeUpTimes[i];
			wakeUpTransmitting[i-1] = wakeUpTransmitting[i];
		}
		// Schedule next time element
		if (wakeUpTimesCounter > 0) {
			scheduleAt(listWakeUpTimes[0], wakeUp);
		}
		break;
	case NodeAppLayer::SEND_REPORT_WITH_CSMA:
		if (!isProcessing) {
			sendReport();
		} else { // If are still calculation the position in Mobile Node type 2
			timeToSend = endProcessingTime + uniform(0, timeReportPhase - processingTime - guardTimeReportPhase, 0);
			scheduleAt(timeToSend, sendExtraReportWithCSMA);
			// Wake Up the node timeSleepToRX time before
			goToWakeUp(timeToSend - timeSleepToRX, true);
			// Go to sleep now if the time to the next report bigger than transition times
			if (timeToSend >= (simTime() + timeSleepToRX + timeRXToSleep)) {
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
		}
		break;
	case NodeAppLayer::SEND_SYNC_TIMER_WITH_CSMA:
		sendBroadcast();
		if (numberOfBroadcastsCounter < numberOfBroadcasts) {
			// Schedule the next random broadcast, we have to subtract first the previous random time to be at the begining of the phase
			// and then add the new random time (relative to the start of VIP phase or Report Phase for type 3 and 4)
			timeToSend = simTime() - randomTransTimes[numberOfBroadcastsCounter - 1] + randomTransTimes[numberOfBroadcastsCounter];
			scheduleAt(timeToSend, sendSyncTimerWithCSMA);
			// Wake Up the node timeSleepToRX time before
			goToWakeUp(timeToSend - timeSleepToRX, true);
			numberOfBroadcastsCounter ++;
		} else {
			numberOfBroadcastsCounter = 0;
		}
		break;
	case NodeAppLayer::CALCULATE_POSITION:
		// -----------------------------------------------------------------------------------------------------
		// - Here we would calculate the position and store it in the FIFO, now we just save our real position -
		// -----------------------------------------------------------------------------------------------------
		if (!isProcessing) {
			insertElementInFIFO(node->pos);
			isProcessing = true;
			endProcessingTime = simTime() + processingTime;
			scheduleAt(endProcessingTime, calculatePosition);
		} else {
			isProcessing = false;
			energy->updateCalculatingTime(processingTime);
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
				timeToSend = simTime() + (timeSyncPhase / 2) + uniform(0, timeReportPhase - guardTimeReportPhase, 0);
				scheduleAt(timeToSend, sendExtraReportWithCSMA);
				// Wake Up the node timeSleepToRX time before
				goToWakeUp(timeToSend - timeSleepToRX, true);
			}
			// When the Mobile Node is type 4 and the phase it sends a request, it doesn't send the broadcast if it was an active phase,
			// This is because the sending from this broadcasts will interfere the waiting time, it is also useful to take out some traffic
			// when the request packet is sent, as this packet is really important to arrive due to the configurations. It makes also the design easier
			if ((nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (activePhase)) {
				// If we are in an active phase, we cancel the broadcasts in this full phase (period) to save battery
				timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
				cancelEvent(sendSyncTimerWithCSMA);
				numberOfBroadcastsCounter = 0;
				// But we have to cancel also the waking up before this broadcast
				for (int i = 0; i < wakeUpTimesCounter; i++) { // Recover the array
					if (listWakeUpTimes[i] == timeToSend - timeSleepToRX) {
						// If we found the element, we erase the element moving all the other elements one position to the front
						for (int j = i+1; j < wakeUpTimesCounter; j++) {
							listWakeUpTimes[j-1] = listWakeUpTimes[j];
							wakeUpTransmitting[j-1] = wakeUpTransmitting[j];
						}
						wakeUpTimesCounter--;
						if (wakeUp->isScheduled()) { // If the event is already scheduled, cancel it to reschedule the one that occurs sooner
							cancelEvent(wakeUp);
						}
						scheduleAt(listWakeUpTimes[0], wakeUp);
					}
				}
			}
		} else {
			EV << "Waiting time from an answer from Anchor addr " << anchorDestinationRequest << " finished." <<endl;
			waitForAnchor = 0;
			anchorDestinationRequest = 0;
			nbRequestWihoutAnswer++;
			canSleep = false; // Makes the node not to sleep unless it can
			if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
				// Sleep the node if the following conditions fulfill
				canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
				// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendSyncTimerWithCSMA->isScheduled()) {
					timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
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
			// Configuring the normal way of working for the active phases in the mobile nodes
			if ((offsetPhasesCounter >= offsetPhases) && (inactivePhasesCounter	== inactivePhases) && (activePhases > 0)) { // Offset already reached and inactive phases done and active phases activated
				activePhasesCounter++;
				activePhase = true;
				if ((activePhasesCounter == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					EV << "Configuring the node for the Sync Phase 1 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					// Before we start reading RSSI again, we have to reset the previous values to get the new ones
					listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					if (offsetSyncPhases >= 1) { // Check if there is an offset not to listen the first sync phase
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
					} else {
						syncListen = true; // Activate to listen
					}
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeVIPPhase - guardTimeVIPPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + timeReportPhase +randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					case NodeAppLayer::LISTEN_TRANSMIT_REPORT: // Mobile Node type 4
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else if (activePhases > activePhasesCounter) { // If we are in an active phase, but not the last one
					EV << "Configuring the node for the Sync Phase 1 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					syncListen = true; // Activate to listen
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeVIPPhase - guardTimeVIPPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					case NodeAppLayer::LISTEN_TRANSMIT_REPORT: // Mobile Node type 4
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else { // We are in the last active phase
					EV << "Configuring the node for the Sync Phase 1 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					if (activePhases == 1) { // If we have only one active phase, the first is the last one also
						// Before we start reading RSSI again, we have to reset the previous values to get the new ones
						listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					}
					syncListen = true; // Activate to listen
					switch(nodeConfig)
					{
					case NodeAppLayer::LISTEN_AND_REPORT: // Mobile Node type 1
						// Schedule of the Report transmission, note that we leave a guard band at the end
						timeToSend = simTime() + timeSyncPhase + uniform(0, timeReportPhase - guardTimeReportPhase, 0);
						scheduleAt(timeToSend, sendReportWithCSMA);
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						break;
					case NodeAppLayer::LISTEN_AND_CALCULATE: // Mobile Node type 2
						// Makes an event to calculate the position at start from next Report phase
						scheduleAt(simTime() + timeSyncPhase, calculatePosition);
						break;
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						createRandomBroadcastTimes(timeVIPPhase - guardTimeVIPPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + timeReportPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					case NodeAppLayer::LISTEN_TRANSMIT_REPORT: // Mobile Node type 4
						// Schedule of the Report transmission, note that we leave a guard band at the end
						type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
						scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendReportWithCSMA);
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(simTime() + timeSyncPhase + type4ReportTime - timeSleepToRX, true);
						// Calculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
						// We have to leave also this minimum separation with the report just generated in type4ReportTime
						createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
						for (int i = 0; i < numberOfBroadcasts; i++)
							EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + randomTransTimes[i] << endl;
						// Schedule of the First Random Broadcast transmission
						timeToSend = simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter];
						scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(timeToSend - timeSleepToRX, true);
						numberOfBroadcastsCounter ++;
						break;
					}
				}
			} else if (offsetPhasesCounter < offsetPhases){ // Offset not reached, increase counter
				offsetPhasesCounter++;
				activePhase = false;
				syncListen = false; // Don't listen this sync phase
				goToSleep(simTime());
				EV << "Offset phase " << offsetPhasesCounter << " from " << offsetPhases << endl;
			} else if (activePhases == 0) {
				EV << "This node has no active phases, the only behavior is through extra reports" << endl;
				activePhase = false;
				syncListen = false; // Don't listen this sync phase
				goToSleep(simTime());
			} else { // Inactive phases
				inactivePhasesCounter++;
				activePhase = false;
				syncListen = false; // Don't listen this sync phase
				goToSleep(simTime());
				EV << "Inactive phase " << inactivePhasesCounter << " from " << inactivePhases << endl;
			}
			// Configuring the extra reports or reports depending on the mobile node type
			if ((offsetReportPhasesCounter >= offsetReportPhases) && (reportPhasesCounter == reportPhases) && (reportPhases > 0)) { // Offset already reached
				reportPhasesCounter = 1;
				syncListenReport = true;
				// If the node was configured to sleep previously, undo it as it has to listen this sync phase
				if (syncListen == false) {
					// If the event is already scheduled, cancel it to reschedule the new one
					if (sleep->isScheduled()) {
						cancelEvent(sleep);
					}
					// Erase this time element
					sleepTimesCounter--;
					for (int i = 1; i <= sleepTimesCounter; i++) { // Remove the first element (the smaller)
						listSleepTimes[i-1] = listSleepTimes[i];
					}
					// Schedule next time element
					if (sleepTimesCounter > 0) {
						scheduleAt(listSleepTimes[0], sleep);
					}
				}

				if (makeExtraReport) {
					// Check if we are already reading RSSI or not, in case we are, we don't erase the old ones. It is also useful
					// to know if we have to schedule an extra report or if we have already the normal one and we don't have to schedule
					// a new one
					if (!syncListen) { // Any node configuration not listening the first syncPhase
						// Before we start reading RSSI again, we have to reset the previous values to get the new ones
						listRSSI = (RSSIs*)calloc(sizeof(RSSIs), numberOfAnchors);
					}
					switch(nodeConfig)
					{
					case NodeAppLayer::LISTEN_AND_REPORT: // Mobile Node type 1
						if (activePhases == activePhasesCounter) { // If we are in type 1 and 4 and in the last active phase we have already a report
							// Do not schedule another extra report, we have already one in this full phase
						} else { // In any other full phase (period)
							// Schedule of the Extra Report transmission, note that we leave a guard band at the end
							type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
							scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
							// Wake Up the node timeSleepToRX time before
							goToWakeUp(simTime() + timeSyncPhase + type4ReportTime - timeSleepToRX, true);
						}
						break;
					case NodeAppLayer::LISTEN_AND_CALCULATE: // Mobile Node type 2
						// Schedule of the Extra Report transmission, note that we leave a guard band at the end
						type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
						scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(simTime() + timeSyncPhase + type4ReportTime - timeSleepToRX, true);
						break;
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						// Schedule of the Extra Report transmission, note that we leave a guard band at the end
						type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
						scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
						// Wake Up the node timeSleepToRX time before
						goToWakeUp(simTime() + timeSyncPhase + type4ReportTime - timeSleepToRX, true);
						if (activePhase) {
							// If we are in an active phase, we cancel the broadcasts in this full phase (period) to save battery
							timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
							cancelEvent(sendSyncTimerWithCSMA);
							numberOfBroadcastsCounter = 0;
							// But we have to cancel also the waking up before this broadcast
							for (int i = 0; i < wakeUpTimesCounter; i++) { // Recover the array
								if (listWakeUpTimes[i] == timeToSend - timeSleepToRX) {
									// If we found the element, we erase the element moving all the other elements one position to the front
									for (int j = i+1; j < wakeUpTimesCounter; j++) {
										listWakeUpTimes[j-1] = listWakeUpTimes[j];
										wakeUpTransmitting[j-1] = wakeUpTransmitting[j];
									}
									wakeUpTimesCounter--;
									if (wakeUp->isScheduled()) { // If the event is already scheduled, cancel it to reschedule the one that occurs sooner
										cancelEvent(wakeUp);
									}
									scheduleAt(listWakeUpTimes[0], wakeUp);
								}
							}
						}
						break;
					case NodeAppLayer::LISTEN_TRANSMIT_REPORT: // Mobile Node type 4
						if (activePhases == activePhasesCounter) { // If we are in type 4 and in the last active phase we have already a report
							// Do not schedule another extra report, we have already one in this full phase
						} else { // In any other full phase (period)
							// Schedule of the Extra Report transmission, note that we leave a guard band at the end
							type4ReportTime = uniform(0, timeReportPhase - guardTimeReportPhase, 0);
							scheduleAt(simTime() + timeSyncPhase + type4ReportTime, sendExtraReportWithCSMA);
							// Wake Up the node timeSleepToRX time before
							goToWakeUp(simTime() + timeSyncPhase + type4ReportTime - timeSleepToRX, true);
							// Recalculate the numberOfBroadcasts random times to transmit, with a minimum separation of minimumSeparation secs, remember that we leave a guard band at the end
							// We have to do this to take into account for the minimum separation the extra report just generated in type4ReportTime
							createRandomBroadcastTimes(timeReportPhase - guardTimeReportPhase);
							for (int i = 0; i < numberOfBroadcasts; i++)
								EV << "Broadcast random time " << i + 1 << " : " << simTime() + timeSyncPhase + randomTransTimes[i] << endl;
							// Cancel the scheduled First Random Broadcast transmission and schedule it again with the new time
							timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
							cancelEvent(sendSyncTimerWithCSMA);
							// But we have to cancel also the waking up before this broadcast
							for (int i = 0; i < wakeUpTimesCounter; i++) { // Recover the array
								if (listWakeUpTimes[i] == timeToSend - timeSleepToRX) {
									// If we found the element, we erase the element moving all the other elements one position to the front
									for (int j = i+1; j < wakeUpTimesCounter; j++) {
										listWakeUpTimes[j-1] = listWakeUpTimes[j];
										wakeUpTransmitting[j-1] = wakeUpTransmitting[j];
									}
									wakeUpTimesCounter--;
									if (wakeUp->isScheduled()) { // If the event is already scheduled, cancel it to reschedule the one that occurs sooner
										cancelEvent(wakeUp);
									}
									scheduleAt(listWakeUpTimes[0], wakeUp);
								}
							}
							numberOfBroadcastsCounter = 0;
							timeToSend = simTime() + timeSyncPhase + randomTransTimes[numberOfBroadcastsCounter];
							scheduleAt(timeToSend, sendSyncTimerWithCSMA); // Program an event to send the broadcasts
							// Wake Up the node timeSleepToRX time before
							goToWakeUp(timeToSend - timeSleepToRX, true);
							numberOfBroadcastsCounter ++;
						}
						break;
					}
					// Here we activate the ASK Flag every askFrequency extra reports
					askFrequencyCounter ++;
					if (askFrequencyCounter == askFrequency) {
						askFrequencyCounter = 0;
						askForRequest = true; // Mark this flag for the report transmission that is already scheduled
						// Schedule the Request for Information in the next full phase (period) at the middle of 1st sync phase (to have all the parameters already configured, and also time to wake up the node)
						scheduleAt(simTime() + fullPhaseTime + (timeSyncPhase / 2), waitForRequest);
					} else if (askFrequencyCounter > askFrequency) {
						askFrequencyCounter = 0; // We just put the counter to 0 but don't send any ask, this case happens when askFrequency = 0
					}
				}
			} else if (offsetReportPhasesCounter < offsetReportPhases){ // Offset not reached, increase counter
				offsetReportPhasesCounter++;
				syncListenReport = false;
			} else if (reportPhases == 0) { // Wrong configuration
				error ("The reportPhases variable must be bigger than 0");
				syncListenReport = false;
			} else { // Inactive phases
				reportPhasesCounter++;
				syncListenReport = false;
			}
			break;
		case AppLayer::REPORT_PHASE:
			phase = AppLayer::REPORT_PHASE;
			nextPhase = AppLayer::VIP_PHASE;
			nextPhaseStartTime = simTime() + timeReportPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Sleep the node if the first and already scheduled broadcast is transmitted after timeSleepToRX + timeRXToSleep
			canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
			// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendSyncTimerWithCSMA->isScheduled()) {
				timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			// Check if there is a report scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendReportWithCSMA->isScheduled()) {
				timeToSend = sendReportWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			// Check if there is an extra report scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendExtraReportWithCSMA->isScheduled()) {
				timeToSend = sendExtraReportWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
			if (syncListenReport) {
				syncListenReport = false;
			}
			break;
		case AppLayer::VIP_PHASE:
			phase = AppLayer::VIP_PHASE;
			nextPhase = AppLayer::SYNC_PHASE_2;
			nextPhaseStartTime = simTime() + timeVIPPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Sleep the node if the following conditions fulfill
			canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
			// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendSyncTimerWithCSMA->isScheduled()) {
				timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
			// Wake up node for next sync phase 2 if necessary, wake it up timeSleepToRX sec. before
			// If we are in an active phase
			if (activePhase) {
				if ((activePhasesCounter == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					if (offsetSyncPhases < 2) { // If there is no offset in sync phases for the first active phase
						if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
							goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
						}
					}
				} else if ((activePhases > activePhasesCounter)) { // If we are in an active phase, but not the last one
					if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
						goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
					}
				} else { // We are in the last active phase
					// Don't wake the node up
				}
			}
			break;
		case AppLayer::SYNC_PHASE_2:
			phase = AppLayer::SYNC_PHASE_2;
			nextPhase = AppLayer::COM_SINK_PHASE_1;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Configuring the normal way of working for the active phases in the mobile nodes
			if (activePhase) {
				if ((activePhasesCounter == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					EV << "Configuring the node for the Sync Phase 2 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					if (offsetSyncPhases >= 2) { // Check if there is an offset not to listen the second sync phase
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
					} else {
						syncListen = true; // Activate to listen
					}
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else if (activePhases > activePhasesCounter) { // If we are in an active phase, but not the last one
					EV << "Configuring the node for the Sync Phase 2 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					syncListen = true; // Activate to listen
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else { // We are in the last active phase
					EV << "Configuring the node for the Sync Phase 2 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					syncListen = false; // Don't listen this sync phase
					goToSleep(simTime());
				}
			} else {
				EV << "Inactive phase " << inactivePhasesCounter << " from " << inactivePhases << endl;
				syncListen = false; // Don't listen this sync phase
				goToSleep(simTime());
			}
			break;
		case AppLayer::COM_SINK_PHASE_1:
			phase = AppLayer::COM_SINK_PHASE_1;
			nextPhase = AppLayer::SYNC_PHASE_3;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			goToSleep(simTime());
			// Wake up node for next sync phase 3 if necessary, wake it up timeSleepToRX sec. before
			// If we are in an active phase
			if (activePhase) {
				if ((activePhasesCounter == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
						goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
					}
				} else if ((activePhases > activePhasesCounter)) { // If we are in an active phase, but not the last one
					if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
						goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
					}
				} else { // We are in the last active phase
					// Don't wake the node up
				}
			}
			break;
		case AppLayer::SYNC_PHASE_3:
			phase = AppLayer::SYNC_PHASE_3;
			nextPhase = AppLayer::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Configuring the normal way of working for the active phases in the mobile nodes
			if (activePhase) {
				if ((activePhasesCounter == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					EV << "Configuring the node for the Sync Phase 3 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					// Here it is no sense disabling listening like in the previous two phases, because if we disable 3 phases, it's like reducing an active phase
					syncListen = true; // Activate to listen
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else if (activePhases > activePhasesCounter) { // If we are in an active phase, but not the last one
					EV << "Configuring the node for the Sync Phase 3 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					syncListen = true; // Activate to listen
					switch(nodeConfig)
					{
					case NodeAppLayer::VIP_TRANSMIT: // Mobile Node type 3
						syncListen = false; // Don't listen this sync phase
						goToSleep(simTime());
						break;
					default:
						EV << "Don't do anything" << endl;
					}
				} else { // We are in the last active phase
					EV << "Configuring the node for the Sync Phase 3 in " << activePhasesCounter << " active phase from " << activePhases << endl;
					syncListen = false; // Don't listen this sync phase
					goToSleep(simTime());
				}
				if (activePhasesCounter >= activePhases) {
					inactivePhasesCounter = 0; // Next full phase will be inactive if inactivePhases > 0
					activePhasesCounter = 0;
				}
			} else {
				EV << "Inactive phase " << inactivePhasesCounter << " from " << inactivePhases << endl;
				syncListen = false; // Don't listen this sync phase
				goToSleep(simTime());
			}
			break;
		case AppLayer::COM_SINK_PHASE_2:
			phase = AppLayer::COM_SINK_PHASE_2;
			nextPhase = AppLayer::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			goToSleep(simTime());
			// Wake up node for next sync phase 1 if necessary, wake it up timeSleepToRX sec. before
			// If there is an extra report in next full phase (period)
			if ((offsetReportPhasesCounter >= offsetReportPhases) && (reportPhasesCounter == reportPhases) && (reportPhases > 0)) {
				goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
			}
			// If next full phase (period) will be an active phase
			if ((offsetPhasesCounter >= offsetPhases) && (inactivePhasesCounter	== inactivePhases) && (activePhases > 0)) {
				if (((activePhasesCounter + 1) == 1) && (activePhases > 1)) { // If we are in the first active phase and there are more than one consecutive phases
					if (offsetSyncPhases < 1) { // If there is no offset in sync phases for the first active phase
						if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
							goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
						}
					}
				} else {
					if (nodeConfig != NodeAppLayer::VIP_TRANSMIT) {
						goToWakeUp(nextPhaseStartTime - timeSleepToRX, false);
					}
				}
			}
			break;
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
	switch(phase)
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
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsBackOffMax++;
			canSleep = false; // Makes the node not to sleep unless it can
			if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
				// Sleep the node if the following conditions fulfill
				canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
				// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendSyncTimerWithCSMA->isScheduled()) {
					timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if there is a report scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendReportWithCSMA->isScheduled()) {
					timeToSend = sendReportWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if there is an extra report scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendExtraReportWithCSMA->isScheduled()) {
					timeToSend = sendExtraReportWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if the begin of the sync phase 2 after VIP phase is too close to go to sleep
				if (phase == AppLayer::VIP_PHASE) {
					if (nextPhaseStartTime < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
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
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsNoACKMax++;
			canSleep = false; // Makes the node not to sleep unless it can
			if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
				// Sleep the node if the following conditions fulfill
				canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
				// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendSyncTimerWithCSMA->isScheduled()) {
					timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if there is a report scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendReportWithCSMA->isScheduled()) {
					timeToSend = sendReportWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if there is an extra report scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendExtraReportWithCSMA->isScheduled()) {
					timeToSend = sendExtraReportWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
				// Check if the begin of the sync phase 2 after VIP phase is too close to go to sleep
				if (phase == AppLayer::VIP_PHASE) {
					if (nextPhaseStartTime < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
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
		canSleep = false; // Makes the node not to sleep unless it can
		if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
			// Sleep the node if the following conditions fulfill
			canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
			// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendSyncTimerWithCSMA->isScheduled()) {
				timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			// Check if there is a report scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendReportWithCSMA->isScheduled()) {
				timeToSend = sendReportWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			// Check if there is an extra report scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendExtraReportWithCSMA->isScheduled()) {
				timeToSend = sendExtraReportWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
			// Check if the begin of the sync phase 2 after VIP phase is too close to go to sleep
			if (phase == AppLayer::VIP_PHASE) {
				if (nextPhaseStartTime < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
		}
		if (canSleep) { // If after all the conditions it is true, we sleep the node
			goToSleep(simTime());
		} else { // If we don't go to sleep is because another event is comming
			EV << "Don't sleeping as another sending or event is coming soon" << endl;
			phy->setRadioState(Radio::RX);
			energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
		}
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
		} else { // We try to sleep the node only if this report is not a request
			canSleep = false; // Makes the node not to sleep unless it can
			if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
				// Sleep the node if the following conditions fulfill
				canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
				// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
				if (sendSyncTimerWithCSMA->isScheduled()) {
					timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
					if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
						canSleep = false;
					}
				}
			}
			if (canSleep) { // If after all the conditions it is true, we sleep the node
				goToSleep(simTime());
			} else { // If we don't go to sleep is because another event is comming
				EV << "Don't sleeping as another sending or event is coming soon" << endl;
				phy->setRadioState(Radio::RX);
				energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
			}
		}
		delete pkt;
		break;
	case BaseMacLayer::ACK_SENT:
		// When the mobile node receives a Report from the Selected Anchor, it answers with an ACK and in that moment we sleep the node if the conditions are satisfied
		canSleep = false; // Makes the node not to sleep unless it can
		if (transfersQueue.isEmpty()) { // If the queue is empty, if not the node cannot sleep
			// Sleep the node if the following conditions fulfill
			canSleep = true; // Activate the sleep variable, we will make it false if some condition is not fulfilled
			// Check if there is a broadcast scheduled sooner than timeSleepToRX + timeRXToSleep
			if (sendSyncTimerWithCSMA->isScheduled()) {
				timeToSend = sendSyncTimerWithCSMA->getArrivalTime();
				if (timeToSend < (simTime() + timeSleepToRX + timeRXToSleep)) {
					canSleep = false;
				}
			}
		}
		if (canSleep) { // If after all the conditions it is true, we sleep the node
			goToSleep(simTime());
		} else { // If we don't go to sleep is because another event is comming
			EV << "Don't sleeping as another sending or event is coming soon" << endl;
			phy->setRadioState(Radio::RX);
			energy->updateStateStatus(true, EnergyConsumption::MAC_IDLE_1, Radio::RX);
		}
		break;
	}
	delete msg;
}

void NodeAppLayer::sendBroadcast()
{
	ApplPkt *pkt = new ApplPkt("Broadcast Message with CSMA", SYNC_MESSAGE_WITH_CSMA);

	pkt->setNumberOfBroadcasts(numberOfBroadcasts);
	pkt->setBitLength(broadcastPacketLength);
	//pkt->setBitLength(88);
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
	int listenedRSSI = 0; // Store how much RSSI did we listened to

	ApplPkt *pkt = new ApplPkt("Report with CSMA", REPORT_WITH_CSMA);

	// Print all the RSSI from all the AN and looks for the bigger to get the selected Anchor
	for (int i = 0; i < (numberOfAnchors); i++) {
		EV << "RSSI: " << (listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI) << " en Anchor " << i << endl;
		if ((listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI) > highestRSSI) {
			highestRSSI = listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI;
			selectedAnchor = i;
		}
		if ((listRSSI[i].RSSIAdition / listRSSI[i].counterRSSI) > 0) {
			listenedRSSI ++;
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
		pkt->setSrcAddr(myNetwAddr);
		pkt->setRealSrcAddr(myNetwAddr);
		pkt->setCSMA(true);

		if (nodeConfig != NodeAppLayer::LISTEN_AND_CALCULATE) { // If MN is not type 2 then the report will be a normal one with measurements info to add to packetsize
			pkt->setBitLength(normalReportPacketLength + 2*8*listenedRSSI); // 2 bytes per RSSI
			//pkt->setBitLength(88);
			EV << "I read " << listenedRSSI << " RSSIs" << endl;
		} else { // It is type 2, in the extra report sends the position calculations it made, we have to change packet size according to this
			pkt->setBitLength(type2ReportPacketLength + 6*8*positionsSavedCounter); // 6 bytes per position
			//pkt->setBitLength(88);
			EV << "I sent " << positionsSavedCounter << " positions" << endl;
			int old_positionsSavedCounter = positionsSavedCounter; // positionsSavedCounter changes during the for, that is why making a copy
			for (int i = 0; i < old_positionsSavedCounter; i++) {
				extractElementFIFO(); // Empties the queue with positions
			}
		}

		// FLAG management
		if (askForRequest) {
			pkt->setAskForRequest(true); // Set Ask Flag = true
			anchorDestinationRequest = netwAddr; // Copies the selected Anchor Netw Addr to have it available in the request report
			EV << "Selected Anchor for ASK Report with MAC Addr: " << anchorDestinationRequest << endl;
			requestPacket = true;
			askForRequest = false;
	        pkt->setBitLength(pkt->getBitLength() + askReportPacketLength);
		} else if (requestPacket) {
			pkt->setRequestPacket(true); // Set Request Flag = true
			pkt->setDestAddr(anchorDestinationRequest); // Redefine Dest Addr
			pkt->setRealDestAddr(anchorDestinationRequest); // Redefine Real Dest Addr
			EV << "Selected Anchor for Request Report with MAC Addr: " << anchorDestinationRequest << endl;
			requestPacket = false;
	        pkt->setBitLength(pkt->getBitLength() + requestPacketLength);
		} else {
		}

		EV << "Send Report to Anchor with Netw Addr " << pkt->getDestAddr() << ", with end destination Netw Addr " << pkt->getRealDestAddr() << " a packet of size " << pkt->getBitLength() / 8 << " bytes" << endl;
		transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
		sendDown(pkt);

	} else { // We didn't have any RSSI value or old selected Anchor
    	EV << "Report not sent as there was no RSSI values to obtain the selected Anchor" << endl;
    	delete pkt;
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
			if ((nodeConfig == NodeAppLayer::LISTEN_TRANSMIT_REPORT) && (syncListenReport || ((activePhases == activePhasesCounter) && (activePhases > 0)))) {
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

void NodeAppLayer::orderVectorMinToMax(simtime_t* times, int counter)
{
	for (int i = 0; i < counter; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (times[i] < times[j])
			{
				simtime_t temp = times[i];
				times[i] = times[j];
				times[j] = temp;
			}
		}
	}
}

void NodeAppLayer::orderVectorMinToMax(simtime_t* times, bool* transmitting, int counter)
{
	for (int i = 0; i < counter; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (times[i] < times[j])
			{
				simtime_t temp = times[i];
				bool temp2 = transmitting[i];
				times[i] = times[j];
				transmitting[i] = transmitting[j];
				times[j] = temp;
				transmitting[j] = temp2;
			}
		}
	}
}

void NodeAppLayer::goToSleep(simtime_t time)
{
	if (!findElement(listSleepTimes, time, sleepTimesCounter)) {
		// Schedule the node to sleep
		listSleepTimes[sleepTimesCounter] = time;
		sleepTimesCounter++;
		orderVectorMinToMax(listSleepTimes, sleepTimesCounter);
		if (sleep->isScheduled()) { // If the event is already scheduled, cancel it to reschedule the one that occurs sooner
			cancelEvent(sleep);
		}
		scheduleAt(listSleepTimes[0], sleep);
	}
}

void NodeAppLayer::goToWakeUp(simtime_t time, bool transmitting)
{
	if (time >= simTime()) { // Not to schedule events in the past
		if (!findElement(listWakeUpTimes, time, wakeUpTimesCounter)) {
			EV << "Wake up in " << time << " s"<< endl;
			// Schedule the node to wake up
			listWakeUpTimes[wakeUpTimesCounter] = time;
			wakeUpTransmitting[wakeUpTimesCounter] = transmitting;
			wakeUpTimesCounter++;
			orderVectorMinToMax(listWakeUpTimes, wakeUpTransmitting, wakeUpTimesCounter);
			if (wakeUp->isScheduled()) { // If the event is already scheduled, cancel it to reschedule the one that occurs sooner
				cancelEvent(wakeUp);
			}
			scheduleAt(listWakeUpTimes[0], wakeUp);
		}
	} else if (phy->getRadioState() == Radio::SLEEP) { // If we are still sleeping and should have woken up before present, means we shouldn't be sleeping
		error ("The node was sleeping when it should have wait awake");
	}
}

bool NodeAppLayer::findElement(simtime_t* times, simtime_t time, int counter)
{
	bool found = false;
	for (int i = 0; i < counter; i++) {
		if (times[i] == time)
			found = true;
	}
	return found;
}
