/* -*- mode:c++ -*- ********************************************************
 * file:        csma.cc
 *
 * author:      Jerome Rousselot, Marcel Steine, Amre El-Hoiydi,
 * 				Marc Loebbers, Yosia Hadisusanto, Andreas Koepke
 *
 * copyright:   (C) 2007-2009 CSEM SA
 * 				(C) 2009 T.U. Eindhoven
 *				(C) 2004,2005,2006
 *              Telecommunication Networks Group (TKN) at Technische
 *              Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 * Funding: This work was partially financed by the European Commission under the
 * Framework 6 IST Project "Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/

#include "csma.h"
#include "FWMath.h"
#include <cassert>
#include <BaseDecider.h>
#include <DeciderResult802154Narrow.h>
#include <BaseArp.h>
#include <MacToPhyControlInfo.h>
#include <PhyToMacControlInfo.h>
#include <NetwToMacControlInfo.h>
#include <MacToNetwControlInfo.h>
#include <SimpleAddress.h>

#include <BaseConnectionManager.h>
//#include <Consts802154.h>

Define_Module(csma);

/**
 * Initialize the of the omnetpp.ini variables in stage 1. In stage
 * two subscribe to the RadioState.
 */

void csma::initialize(int stage) {
	BaseMacLayer::initialize(stage);

	if (stage == 0) {

		useMACAcks = par("useMACAcks").boolValue();
		queueLength = par("queueLength");
		sifs = par("sifs");
		transmissionAttemptInterruptedByRx = false;
		nbTxFrames = 0;
		nbRxFrames = 0;
		nbMissedAcks = 0;
		nbTxAcks = 0;
		nbRecvdAcks = 0;
		nbDroppedFrames = 0;
		nbDuplicates = 0;
		nbBackoffs = 0;
		backoffValues = 0;
		nbDroppedFromMACQueueNoTimeInPhase = 0;
		nbDroppedMACNoTimeBeforePhaseEnd = 0;
		stats = par("stats");
		trace = par("trace");
		macMaxCSMABackoffs = par("macMaxCSMABackoffs");
		macMaxFrameRetries = par("macMaxFrameRetries");
		macAckWaitDuration = par("macAckWaitDuration").doubleValue();
		aUnitBackoffPeriod = par("aUnitBackoffPeriod");
		ccaDetectionTime = par("ccaDetectionTime").doubleValue();
	   	LIFS = par("LIFS").doubleValue();
		rxSetupTime = par("rxSetupTime").doubleValue();
		aTurnaroundTime = par("aTurnaroundTime").doubleValue();
		bitrate = par("bitrate");
		ackLength = par("ackLength");
		ackMessage = NULL;

		//init parameters for backoff method
		std::string backoffMethodStr = par("backoffMethod").stdstringValue();
		if(backoffMethodStr == "exponential") {
			backoffMethod = EXPONENTIAL;
			macMinBE = par("macMinBE");
			macMaxBE = par("macMaxBE");
		}
		else {
			if(backoffMethodStr == "linear") {
				backoffMethod = LINEAR;
			}
			else if (backoffMethodStr == "constant") {
				backoffMethod = CONSTANT;
			}
			else {
				error("Unknown backoff method \"%s\".\
					   Use \"constant\", \"linear\" or \"\
					   \"exponential\".", backoffMethodStr.c_str());
			}
			initialCW = par("contentionWindow");
		}
		NB = 0;

		txPower = par("txPower").doubleValue();

		droppedPacket.setReason(DroppedPacket::NONE);
		nicId = getParentModule()->getId();

		catDroppedPacket = utility->getCategory(&droppedPacket);

		// initialize the timers
		backoffTimer = new cMessage("timer-backoff");
		ccaTimer = new cMessage("timer-cca");
		sifsTimer = new cMessage("timer-sifs");
		rxAckTimer = new cMessage("timer-rxAck");
		macState = IDLE_1;
		txAttempts = 0;

	} else if(stage == 2) {
		cc = getConnectionManager();

    	if(cc->hasPar("pMax") && txPower > cc->par("pMax").doubleValue())
            opp_error("TranmitterPower can't be bigger than pMax in ConnectionManager! "
            		  "Please adjust your omnetpp.ini file accordingly.");

    	// Assign the type of host to 3 (computer)
		node = cc->findNic(getParentModule()->getId());

		if (node->moduleType == 2) { // Only for Mobile Nodes
	    	energy = static_cast<EnergyConsumption*>(getParentModule()->getParentModule()->getSubmodule("energy"));
		}

		EV << "queueLength = " << queueLength
		<< " bitrate = " << bitrate
		<< " backoff method = " << par("backoffMethod").stringValue() << endl;

		EV << "finished csma init stage 1." << endl;

	} else if (stage == 4) { // Modified by Jorge, in this stage we calculate the necessary things to cancel the queue and don't permit to schedule more packets after a determined time
		beginPhases = new cMessage("next-phase",csma::EV_BEGIN_PHASE);
		nextPhase = csma::REPORT_PHASE;
		syncPacketsPerSyncPhase = getParentModule()->getParentModule()->getParentModule()->getSubmodule("computer", 0)->getSubmodule("appl")->par("syncPacketsPerSyncPhase");
		syncPacketTime = getParentModule()->getParentModule()->getParentModule()->getSubmodule("computer", 0)->getSubmodule("appl")->par("syncPacketTime");
		phase2VIPPercentage = getParentModule()->getParentModule()->getParentModule()->getSubmodule("computer", 0)->getSubmodule("appl")->par("phase2VIPPercentage");
		fullPhaseTime = getParentModule()->getParentModule()->getParentModule()->par("fullPhaseTime");
		timeComSinkPhase = getParentModule()->getParentModule()->getParentModule()->par("timeComSinkPhase");
		smallTime = 0.000001;			//  1 us
		guardTransmitTime = 0.002;		//  2 ms, we use this time as guard time at the end of every phase
		timeFromBackOffToTX = ccaDetectionTime + aTurnaroundTime + rxSetupTime + guardTransmitTime;

		computer = cc->findNic(getParentModule()->getParentModule()->getParentModule()->getSubmodule("computer", 0)->findSubmodule("nic"));
		timeSyncPhase = syncPacketsPerSyncPhase * computer->numTotalSlots * syncPacketTime ;
		timeVIPPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * timeSyncPhase)) * phase2VIPPercentage;
		timeReportPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * timeSyncPhase)) * (1 - phase2VIPPercentage);
		nextPhaseStartTime = simTime() + timeSyncPhase - smallTime;
		scheduleAt(nextPhaseStartTime, beginPhases);
	}
}

void csma::finish() {
	if (stats) {
		recordScalar("nbTxFrames", nbTxFrames);
		recordScalar("nbRxFrames", nbRxFrames);
		recordScalar("nbDroppedFrames", nbDroppedFrames);
		recordScalar("nbMissedAcks", nbMissedAcks);
		recordScalar("nbRecvdAcks", nbRecvdAcks);
		recordScalar("nbTxAcks", nbTxAcks);
		recordScalar("nbDuplicates", nbDuplicates);
		if (nbBackoffs > 0) {
			recordScalar("meanBackoff", backoffValues / nbBackoffs);
		} else {
			recordScalar("meanBackoff", 0);
		}
		recordScalar("nbBackoffs", nbBackoffs);
		recordScalar("backoffDurations", backoffValues);
		recordScalar("nbDroppedFromMACQueueNoTimeInPhase", nbDroppedFromMACQueueNoTimeInPhase);
		recordScalar("nbDroppedMACNoTimeBeforePhaseEnd",nbDroppedMACNoTimeBeforePhaseEnd);
	}
}

csma::~csma() {
	cancelAndDelete(backoffTimer);
	cancelAndDelete(ccaTimer);
	cancelAndDelete(sifsTimer);
	cancelAndDelete(rxAckTimer);
	if (ackMessage)
		delete ackMessage;
	MacQueue::iterator it;
	for (it = macQueue.begin(); it != macQueue.end(); ++it) {
		delete (*it);
	}
	cancelAndDelete(beginPhases);
}

/**
 * Encapsulates the message to be transmitted and pass it on
 * to the FSM main method for further processing.
 */
void csma::handleUpperMsg(cMessage *msg) {
	//MacPkt *macPkt = encapsMsg(msg);
	MacPkt *macPkt = new MacPkt(msg->getName());
	macPkt->setBitLength(headerLength);
	NetwToMacControlInfo* cInfo = static_cast<NetwToMacControlInfo*> (msg->removeControlInfo());
	EV<<"CSMA received a message from upper layer, name is " << msg->getName() <<", CInfo removed, mac addr="<< cInfo->getNextHopMac()<<endl;
	int dest = cInfo->getNextHopMac();
	macPkt->setCsmaActive(cInfo->getCsmaActive());
	macPkt->setDestAddr(dest);
	delete cInfo;
	macPkt->setSrcAddr(myMacAddr);

	if(useMACAcks) {
		if(SeqNrParent.find(dest) == SeqNrParent.end()) {
			//no record of current parent -> add next sequence number to map
			SeqNrParent[dest] = 1;
			macPkt->setSequenceId(0);
			EV << "Adding a new parent to the map of Sequence numbers:" << dest << endl;
		}
		else {
			macPkt->setSequenceId(SeqNrParent[dest]);
			EV << "Packet send with sequence number = " << SeqNrParent[dest] << endl;
			SeqNrParent[dest]++;
		}
	}

	//RadioAccNoise3PhyControlInfo *pco = new RadioAccNoise3PhyControlInfo(bitrate);
	//macPkt->setControlInfo(pco);
	assert(static_cast<cPacket*>(msg));
	EV <<"pkt preencapsulated, length: " << macPkt->getBitLength() << "\n";
	macPkt->encapsulate(static_cast<cPacket*>(msg));
	EV <<"pkt encapsulated, length: " << macPkt->getBitLength() << "\n";
	executeMac(EV_SEND_REQUEST, macPkt);
}

void csma::updateStatusIdle(t_mac_event event, cMessage *msg) {
	switch (event) {
	case EV_SEND_REQUEST:
		if (macQueue.size() <= queueLength) {
			//BE = macMinBE;
			if ((static_cast<MacPkt *> (msg))->getCsmaActive())
			{
				EV<<"(1) FSM State IDLE_1, EV_SEND_REQUEST and [TxBuff avail]: startTimerBackOff -> BACKOFF." << endl;
				NB = 0;
				startTimer(TIMER_BACKOFF);
				if (backoffTimer->isScheduled()) {
					updateMacState(BACKOFF_2);
					if (node->moduleType == 2) { // Only for Mobile Nodes
						energy->updateStateStatus(true, macState, Radio::RX);
					}
					macQueue.push_back(static_cast<MacPkt *> (msg));
				} else {
					manageQueue();
					delete msg;
					nbDroppedMACNoTimeBeforePhaseEnd++;
				}
			}
			else
			{
				EV<<"(1) FSM State IDLE_1, EV_SEND_REQUEST and [TxBuff avail]: startTimerBackOff -> No BACKOFF. Wait for SIFS " << sifs << "s" << endl;
				NB = macMaxCSMABackoffs; // if csma deactivated we drop the packet directly
				scheduleAt(simTime() + sifs, backoffTimer);
				updateMacState(BACKOFF_2);
				if (node->moduleType == 2) {
					energy->updateStateStatus(true, macState, Radio::RX);
				}
				macQueue.push_back(static_cast<MacPkt *> (msg));
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
				// - Here we could have also the same security to see if we have time before end phase as just above but we just use this type in Sync Slotted Packets -
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
			}
		} else {
			// queue is full, message has to be deleted
			EV << "(12) FSM State IDLE_1, EV_SEND_REQUEST and [TxBuff not avail]: dropping packet -> IDLE." << endl;
			msg->setName("MAC ERROR");
			msg->setKind(QUEUE_FULL);
			sendControlUp(msg);
			droppedPacket.setReason(DroppedPacket::QUEUE);
			utility->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
			updateMacState(IDLE_1);
		}
		break;
	case EV_DUPLICATE_RECEIVED:
		EV << "(15) FSM State IDLE_1, EV_DUPLICATE_RECEIVED: setting up radio tx -> WAITSIFS." << endl;
		//sendUp(decapsMsg(static_cast<MacSeqPkt *>(msg)));
		delete msg;
		if(useMACAcks) {
			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		}
		break;

	case EV_FRAME_RECEIVED:
		EV << "(15) FSM State IDLE_1, EV_FRAME_RECEIVED: setting up radio tx -> WAITSIFS." << endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;

		if(useMACAcks) {
			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		}
		break;

	case EV_BROADCAST_RECEIVED:
		EV << "(23) FSM State IDLE_1, EV_BROADCAST_RECEIVED: Nothing to do." << endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusBackoff(t_mac_event event, cMessage *msg) {
	switch (event) {
	case EV_TIMER_BACKOFF:
		EV<< "(2) FSM State BACKOFF, EV_TIMER_BACKOFF:"
		<< " starting CCA timer." << endl;
		startTimer(TIMER_CCA);
		updateMacState(CCA_3);
		EV<< "Radio Status: " << phy->getRadioState()<<endl;
		phy->setRadioState(Radio::RX);
		if (node->moduleType == 2) { // Only for Mobile Nodes
			energy->updateStateStatus(true, macState, Radio::RX);
		}
		break;
	case EV_DUPLICATE_RECEIVED:
		// suspend current transmission attempt,
		// transmit ack,
		// and resume transmission when entering manageQueue()
		EV << "(28) FSM State BACKOFF, EV_DUPLICATE_RECEIVED:";
		if(useMACAcks) {
			EV << "suspending current transmit tentative and transmitting ack";
			transmissionAttemptInterruptedByRx = true;
			cancelEvent(backoffTimer);
			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		} else {
			EV << "Nothing to do.";
		}
		//sendUp(decapsMsg(static_cast<MacSeqPkt *>(msg)));
		delete msg;

		break;
	case EV_FRAME_RECEIVED:
		// suspend current transmission attempt,
		// transmit ack,
		// and resume transmission when entering manageQueue()
		EV << "(28) FSM State BACKOFF, EV_FRAME_RECEIVED:";
		if(useMACAcks) {
			EV << "suspending current transmit tentative and transmitting ack";
			transmissionAttemptInterruptedByRx = true;
			cancelEvent(backoffTimer);

			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		} else {
			EV << "sending frame up and resuming normal operation.";
		}
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	case EV_BROADCAST_RECEIVED:
		EV << "(29) FSM State BACKOFF, EV_BROADCAST_RECEIVED:"
		<< "sending frame up and resuming normal operation." <<endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::attachSignal(MacPkt* mac, simtime_t startTime) {
	simtime_t duration = (mac->getBitLength() + phyHeaderLength)/bitrate;
	EV << "Size: " << mac->getBitLength() + phyHeaderLength <<", Duration: " << duration<<endl;
	Signal* s = createSignal(startTime, duration, txPower, bitrate);
	MacToPhyControlInfo* cinfo = new MacToPhyControlInfo(s);

	mac->setControlInfo(cinfo);
}

void csma::updateStatusCCA(t_mac_event event, cMessage *msg) {
	switch (event) {
	case EV_TIMER_CCA:
	{
		EV<< "(25) FSM State CCA_3, EV_TIMER_CCA" << endl;
		bool isIdle = phy->getChannelState().isIdle();
		if(isIdle) {
			EV << "(3) FSM State CCA_3, EV_TIMER_CCA, [Channel Idle]: -> TRANSMITFRAME_4." << endl;
			updateMacState(TRANSMITFRAME_4);
			phy->setRadioState(Radio::TX);
			if (node->moduleType == 2) { // Only for Mobile Nodes
//				scheduleAt(simTime() + aTurnaroundTime, energyAfterCCA);
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			MacPkt * mac = check_and_cast<MacPkt *>(macQueue.front()->dup());
			attachSignal(mac, simTime() + aTurnaroundTime);
			//sendDown(msg);
			// give time for the radio to be in Tx state before transmitting
			sendDelayed(mac, aTurnaroundTime, lowerGateOut);
			nbTxFrames++;
		} else {
			// Channel was busy, increment 802.15.4 backoff timers as specified.
			EV << "(7) FSM State CCA_3, EV_TIMER_CCA, [Channel Busy]: "
			<< " increment counters." << endl;
			NB = NB+1;
			//BE = std::min(BE+1, macMaxBE);

			// decide if we go for another backoff or if we drop the frame
			if(NB> macMaxCSMABackoffs) {
				// drop the frame
				EV << "Tried " << NB << " backoffs, all reported a busy "
				<< "channel. Dropping the packet." << endl;
				cMessage * mac = macQueue.front();
				macQueue.pop_front();
				txAttempts = 0;
				nbDroppedFrames++;
				mac->setName("MAC BACKOFF ERROR");
				mac->setKind(PACKET_DROPPED_BACKOFF);
				sendControlUp(mac);
				manageQueue();
			} else {
				// redo backoff
				// Modified by Jorge
				startTimer(TIMER_BACKOFF);
				if (backoffTimer->isScheduled()) {
					updateMacState(BACKOFF_2);
					if (node->moduleType == 2) { // Only for Mobile Nodes
						energy->updateStateStatus(true, macState, Radio::RX);
					}
				} else {
					cMessage *mac = macQueue.front();
					macQueue.pop_front();
					delete mac;
					manageQueue();
					nbDroppedMACNoTimeBeforePhaseEnd++;
				}
			}
		}
		break;
	}
	case EV_DUPLICATE_RECEIVED:
		EV << "(26) FSM State CCA_3, EV_DUPLICATE_RECEIVED:";
		if(useMACAcks) {
			EV << " setting up radio tx -> WAITSIFS." << endl;
			// suspend current transmission attempt,
			// transmit ack,
			// and resume transmission when entering manageQueue()
			transmissionAttemptInterruptedByRx = true;
			cancelEvent(ccaTimer);

			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		} else {
			EV << " Nothing to do." << endl;
		}
		//sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;

	case EV_FRAME_RECEIVED:
		EV << "(26) FSM State CCA_3, EV_FRAME_RECEIVED:";
		if(useMACAcks) {
			EV << " setting up radio tx -> WAITSIFS." << endl;
			// suspend current transmission attempt,
			// transmit ack,
			// and resume transmission when entering manageQueue()
			transmissionAttemptInterruptedByRx = true;
			cancelEvent(ccaTimer);
			phy->setRadioState(Radio::TX);
			updateMacState(WAITSIFS_6);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::TX);
			}
			startTimer(TIMER_SIFS);
		} else {
			EV << " Nothing to do." << endl;
		}
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	case EV_BROADCAST_RECEIVED:
		EV << "(24) FSM State BACKOFF, EV_BROADCAST_RECEIVED:"
		<< " Nothing to do." << endl;
		sendUp(decapsMsg(static_cast<MacPkt *>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusTransmitFrame(t_mac_event event, cMessage *msg) {
	if (event == EV_FRAME_TRANSMITTED) {
		//    delete msg;
		MacPkt * packet = macQueue.front();

		bool expectAck = useMACAcks;
		if (packet->getDestAddr() != L2BROADCAST) {
			//unicast
			EV << "(4) FSM State TRANSMITFRAME_4, "
			   << "EV_FRAME_TRANSMITTED [Unicast]: ";
		} else {
			//broadcast
			EV << "(27) FSM State TRANSMITFRAME_4, EV_FRAME_TRANSMITTED "
			   << " [Broadcast]";
			expectAck = false;
		}

		if(expectAck) {
			EV << "RadioSetupRx -> WAITACK." << endl;
			updateMacState(WAITACK_5);
			phy->setRadioState(Radio::RX);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(false, macState, Radio::RX);
			}
			startTimer(TIMER_RX_ACK);
		} else {
			EV << ": RadioSetupRx, manageQueue..." << endl;
			macQueue.pop_front();
			if (node->moduleType != 2) { // The mobile node changes this state in the App Layer after ControlDown ACK
				phy->setRadioState(Radio::RX);
			}
			packet->setName("SYNC SENT");
			packet->setKind(SYNC_SENT);
			sendControlUp(packet);
			manageQueue();
		}
	} else {
		fsmError(event, msg);
	}
}

void csma::updateStatusWaitAck(t_mac_event event, cMessage *msg) {
	assert(useMACAcks);

	cMessage * mac;
	switch (event) {
	case EV_ACK_RECEIVED:
		EV<< "(5) FSM State WAITACK_5, EV_ACK_RECEIVED: "
		<< " ProcessAck, manageQueue..." << endl;
		if(rxAckTimer->isScheduled())
			cancelEvent(rxAckTimer);
		mac = static_cast<cMessage *>(macQueue.front());
		macQueue.pop_front();
		txAttempts = 0;
		mac->setName("MAC SUCCESS");
		mac->setKind(TX_OVER);
		sendControlUp(mac);
		delete msg;
		manageQueue();
		break;
	case EV_ACK_TIMEOUT:
		EV << "(12) FSM State WAITACK_5, EV_ACK_TIMEOUT:"
		<< " incrementCounter/dropPacket, manageQueue..." << endl;
		manageMissingAck(event, msg);
		break;
	case EV_BROADCAST_RECEIVED:
	case EV_FRAME_RECEIVED:
		sendUp(decapsMsg(static_cast<MacPkt*>(msg)));
	case EV_DUPLICATE_RECEIVED:
		EV << "Error ! Received a frame during SIFS !" << endl;
		EV << "EV_DUPLICATE_RECEIVED" << endl;
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}

}

void csma::manageMissingAck(t_mac_event event, cMessage *msg) {
	if (txAttempts < macMaxFrameRetries + 1) {
		// increment counter
		txAttempts++;
		EV<< "I will retransmit this packet (I already tried "
		<< txAttempts << " times)." << endl;
	} else {
		// drop packet
		EV << "Packet was transmitted " << txAttempts
		<< " times and I never got an Ack. I drop the packet." << endl;
		cMessage * mac = macQueue.front();
		macQueue.pop_front();
		txAttempts = 0;
		mac->setName("MAC ACK ERROR");
		mac->setKind(PACKET_DROPPED);
		sendControlUp(mac);
	}
	manageQueue();
}
void csma::updateStatusSIFS(t_mac_event event, cMessage *msg) {
	assert(useMACAcks);

	switch (event) {
	case EV_TIMER_SIFS:
		EV<< "(17) FSM State WAITSIFS_6, EV_TIMER_SIFS:"
		<< " sendAck -> TRANSMITACK." << endl;
		updateMacState(TRANSMITACK_7);
		if (node->moduleType == 2) { // Only for Mobile Nodes
			energy->updateStateStatus(true, macState, Radio::TX);
		}
		attachSignal(ackMessage, simTime());
		sendDown(ackMessage);
		nbTxAcks++;
		//		sendDelayed(ackMessage, aTurnaroundTime, lowergateOut);
		ackMessage = NULL;
		break;
	case EV_TIMER_BACKOFF:
		// Backoff timer has expired while receiving a frame. Restart it
		// and stay here.
		EV << "(16) FSM State WAITSIFS_6, EV_TIMER_BACKOFF. "
		<< "Restart backoff timer and don't move." << endl;
		startTimer(TIMER_BACKOFF);
		if (backoffTimer->isScheduled()) {
			updateMacState(BACKOFF_2);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::RX);
			}
		} else {
			cMessage *mac = macQueue.front();
			macQueue.pop_front();
			delete mac;
			manageQueue();
			nbDroppedMACNoTimeBeforePhaseEnd++;
		}
		break;
	case EV_BROADCAST_RECEIVED:
	case EV_FRAME_RECEIVED:
		EV << "Error ! Received a frame during SIFS !" << endl;
		sendUp(decapsMsg(static_cast<MacPkt*>(msg)));
		delete msg;
		break;
	default:
		fsmError(event, msg);
	}
}

void csma::updateStatusTransmitAck(t_mac_event event, cMessage *msg) {
	assert(useMACAcks);

	if (event == EV_FRAME_TRANSMITTED) {
		EV<< "(19) FSM State TRANSMITACK_7, EV_FRAME_TRANSMITTED:"
		<< " ->manageQueue." << endl;
		if (node->moduleType != 2) { // The mobile node changes this state in the App Layer after ControlDown ACK
			phy->setRadioState(Radio::RX);
		}
		//		delete msg;
		manageQueue();
		// Notify the App layer that we already sent and ACK to confirm the received report
		sendControlUp(new cMessage("ACK SENT", ACK_SENT));
	} else {
		fsmError(event, msg);
	}
}

void csma::updateStatusNotIdle(cMessage *msg) {
	EV<< "(20) FSM State NOT IDLE, EV_SEND_REQUEST. Is a TxBuffer available ?" << endl;
	if (macQueue.size() <= queueLength) {
		macQueue.push_back(static_cast<MacPkt *>(msg));
		EV << "(21) FSM State NOT IDLE, EV_SEND_REQUEST"
		<<" and [TxBuff avail]: enqueue packet and don't move." << endl;
	} else {
		// queue is full, message has to be deleted
		EV << "(22) FSM State NOT IDLE, EV_SEND_REQUEST"
		<< " and [TxBuff not avail]: dropping packet and don't move."
		<< endl;
		msg->setName("MAC ERROR");
		msg->setKind(QUEUE_FULL);
		sendControlUp(msg);
		droppedPacket.setReason(DroppedPacket::QUEUE);
		utility->publishBBItem(catDroppedPacket, &droppedPacket, nicId);
	}

}
/**
 * Updates state machine.
 */
void csma::executeMac(t_mac_event event, cMessage *msg) {
	EV<< "In executeMac" << endl;
	if(macState != IDLE_1 && event == EV_SEND_REQUEST) {
		updateStatusNotIdle(msg);
	} else {
		switch(macState) {
		case IDLE_1:
			updateStatusIdle(event, msg);
			break;
		case BACKOFF_2:
			updateStatusBackoff(event, msg);
			break;
		case CCA_3:
			updateStatusCCA(event, msg);
			break;
		case TRANSMITFRAME_4:
			updateStatusTransmitFrame(event, msg);
			break;
		case WAITACK_5:
			updateStatusWaitAck(event, msg);
			break;
		case WAITSIFS_6:
			updateStatusSIFS(event, msg);
			break;
		case TRANSMITACK_7:
			updateStatusTransmitAck(event, msg);
			break;
		default:
			EV << "Error in CSMA FSM: an unknown state has been reached. macState=" << macState << endl;
		}
	}
}

void csma::manageQueue() {
	if (macQueue.size() != 0) {
		EV<< "(manageQueue) there are " << macQueue.size() << " packets to send, entering backoff wait state." << endl;
		//Modified by Jorge, this if was after the updateMacState but I think must be here to reset the NB and start with minBE
		if( transmissionAttemptInterruptedByRx) {
			// resume a transmission cycle which was interrupted by
			// a frame reception during CCA check
			transmissionAttemptInterruptedByRx = false;
		} else {
			// initialize counters if we start a new transmission
			// cycle from zero
			NB = 0;
			//BE = macMinBE;
		}
		if(! backoffTimer->isScheduled()) {
			startTimer(TIMER_BACKOFF);
		}
		else if (backoffTimer->getTimestamp() + timeFromBackOffToTX >= nextPhaseStartTime) {
			cancelEvent(backoffTimer);
		}
		if (backoffTimer->isScheduled()) {
			updateMacState(BACKOFF_2);
			if (node->moduleType == 2) { // Only for Mobile Nodes
				energy->updateStateStatus(true, macState, Radio::RX);
			}
		} else {
			cMessage *mac = macQueue.front();
			macQueue.pop_front();
			delete mac;
			nbDroppedMACNoTimeBeforePhaseEnd++;
			manageQueue();
		}
	} else {
		EV << "(manageQueue) no packets to send, entering IDLE state." << endl;
		updateMacState(IDLE_1);
	}
}

void csma::updateMacState(t_mac_states newMacState) {
	macState = newMacState;
}

/*
 * Called by the FSM machine when an unknown transition is requested.
 */
void csma::fsmError(t_mac_event event, cMessage *msg) {
	EV<< "FSM Error ! In state " << macState << ", received unknown event:" << event << "." << endl;
	if (msg != NULL)
	delete msg;
}

void csma::startTimer(t_mac_timer timer) {
	if (timer == TIMER_BACKOFF) {
		simtime_t temp = scheduleBackoff();
		if ((temp + timeFromBackOffToTX) < nextPhaseStartTime) {
			scheduleAt(temp, backoffTimer);
		}
	} else if (timer == TIMER_CCA) {
		simtime_t ccaTime = rxSetupTime + ccaDetectionTime;
		EV<< "(startTimer) ccaTimer value=" << ccaTime
		<< "(rxSetupTime,ccaDetectionTime:" << rxSetupTime
		<< "," << ccaDetectionTime <<")." << endl;
		scheduleAt(simTime()+rxSetupTime+ccaDetectionTime, ccaTimer);
	} else if (timer==TIMER_SIFS) {
		assert(useMACAcks);
		EV << "(startTimer) sifsTimer value=" << sifs << endl;
		scheduleAt(simTime()+sifs, sifsTimer);
	} else if (timer==TIMER_RX_ACK) {
		assert(useMACAcks);
		EV << "(startTimer) rxAckTimer value=" << macAckWaitDuration << endl;
		scheduleAt(simTime()+macAckWaitDuration, rxAckTimer);
	} else {
		EV << "Unknown timer requested to start:" << timer << endl;
	}
}

simtime_t csma::scheduleBackoff() {

	simtime_t backoffTime;

	switch(backoffMethod) {
	case EXPONENTIAL:
	{
		int BE = std::min(macMinBE + NB, macMaxBE);
		double d = std::pow((double) 2, (int) BE);
		int v = (int) d - 1;
		int r = intuniform(0, v, 0);
		backoffTime = r * aUnitBackoffPeriod;

		EV<< "(startTimer) backoffTimer value=" << backoffTime
		<< " (BE=" << BE << ", 2^BE-1= " << v << "r="
		<< r << ")" << " + SIFS (" << sifs << ") = " << backoffTime + sifs << endl;
		break;
	}
	case LINEAR:
	{
		int slots = intuniform(1, initialCW + NB, 0);
		backoffTime = slots * aUnitBackoffPeriod;
		EV<< "(startTimer) backoffTimer value=" << backoffTime << endl;
		break;
	}
	case CONSTANT:
	{
		int slots = intuniform(1, initialCW, 0);
		backoffTime = slots * aUnitBackoffPeriod;
		EV<< "(startTimer) backoffTimer value=" << backoffTime << endl;
		break;
	}
	default:
		error("Unknown backoff method!");
	}

	nbBackoffs = nbBackoffs + 1;
	backoffValues = backoffValues + backoffTime.dbl();

	// We add here sifs as offset because when BackOff is 0 we need to leave this time, so we increase this time for all backoffs as a state change
	return backoffTime + simTime() + sifs;
}

/*
 * Binds timers to events and executes FSM.
 */
void csma::handleSelfMsg(cMessage *msg) {
	EV<< "timer routine." << endl;
	if(msg==backoffTimer)
		executeMac(EV_TIMER_BACKOFF, msg);
	else if(msg==ccaTimer)
		executeMac(EV_TIMER_CCA, msg);
	else if(msg==sifsTimer)
		executeMac(EV_TIMER_SIFS, msg);
	else if(msg==rxAckTimer) {
		nbMissedAcks++;
		executeMac(EV_ACK_TIMEOUT, msg);
	} else if(msg==beginPhases) {
		switch (nextPhase)
		{
		case csma::SYNC_PHASE_1:
			nextPhase = csma::REPORT_PHASE;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::REPORT_PHASE:
			nextPhase = csma::VIP_PHASE;
			nextPhaseStartTime = simTime() + timeReportPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::VIP_PHASE:
			nextPhase = csma::SYNC_PHASE_2;
			nextPhaseStartTime = simTime() + timeVIPPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::SYNC_PHASE_2:
			nextPhase = csma::COM_SINK_PHASE_1;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::COM_SINK_PHASE_1:
			nextPhase = csma::SYNC_PHASE_3;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::SYNC_PHASE_3:
			nextPhase = csma::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case csma::COM_SINK_PHASE_2:
			nextPhase = csma::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		}
		// Empty the Queue
		if (macQueue.size() > 0) {
			EV << "Removing " << macQueue.size() << " Queue elements in phase change" << endl;
			nbDroppedFromMACQueueNoTimeInPhase = nbDroppedFromMACQueueNoTimeInPhase + macQueue.size();
			while (macQueue.size() > 0) {
				cMessage *mac = macQueue.front();
				macQueue.pop_front();
				delete mac;
			}
		} else {
			EV << "MAC Queue empty in phase change." << endl;
		}
	} else
		EV << "CSMA Error: unknown timer fired:" << msg << endl;
}

/**
 * Compares the address of this Host with the destination address in
 * frame. Generates the corresponding event.
 */
void csma::handleLowerMsg(cMessage *msg) {
	MacPkt *macPkt = static_cast<MacPkt *> (msg);
	long src = macPkt->getSrcAddr();
	long dest = macPkt->getDestAddr();
	long ExpectedNr = 0;

	EV<< "Received frame name= " << macPkt->getName()
	<< ", myState=" << macState << " src=" << macPkt->getSrcAddr()
	<< " dst=" << macPkt->getDestAddr() << " myAddr="
	<< myMacAddr << endl;

	if(macPkt->getDestAddr() == myMacAddr)
	{
		if(!useMACAcks) {
			EV << "Received a data packet addressed to me." << endl;
			nbRxFrames++;
			executeMac(EV_FRAME_RECEIVED, macPkt);
		}
		else {
			long SeqNr = macPkt->getSequenceId();

			if(strcmp(macPkt->getName(), "CSMA-Ack") != 0) {
				// This is a data message addressed to us
				// and we should send an ack.
				// we build the ack packet here because we need to
				// copy data from macPkt (src).
				EV << "Received a data packet addressed to me,"
				   << " preparing an ack..." << endl;

				nbRxFrames++;

				if(ackMessage != NULL)
					delete ackMessage;
				ackMessage = new MacPkt("CSMA-Ack");
				ackMessage->setSrcAddr(myMacAddr);
				ackMessage->setDestAddr(macPkt->getSrcAddr());
				ackMessage->setBitLength(ackLength);
				//Check for duplicates by checking expected seqNr of sender
				if(SeqNrChild.find(src) == SeqNrChild.end()) {
					//no record of current child -> add expected next number to map
					SeqNrChild[src] = SeqNr + 1;
					EV << "Adding a new child to the map of Sequence numbers:" << src << endl;
					executeMac(EV_FRAME_RECEIVED, macPkt);
				}
				else {
					ExpectedNr = SeqNrChild[src];
					EV << "Expected Sequence number is " << ExpectedNr <<
					" and number of packet is " << SeqNr << endl;
					if(SeqNr < ExpectedNr) {
						//Duplicate Packet, count and do not send to upper layer
						nbDuplicates++;
						EV << "Duplicate Packet Received" << endl;
						executeMac(EV_DUPLICATE_RECEIVED, macPkt);
					}
					else {
						SeqNrChild[src] = SeqNr + 1;
						EV << "Normal Packet Received" << endl;
						executeMac(EV_FRAME_RECEIVED, macPkt);
					}
				}

			} else if(macQueue.size() != 0) {

				// message is an ack, and it is for us.
				// Is it from the right node ?
				MacPkt * firstPacket = static_cast<MacPkt *>(macQueue.front());
				if(macPkt->getSrcAddr() == firstPacket->getDestAddr()) {
					nbRecvdAcks++;
					executeMac(EV_ACK_RECEIVED, macPkt);
				} else {
					EV << "Error! Received an ack from an unexpected source: src=" << macPkt->getSrcAddr() << ", I was expecting from node addr=" << firstPacket->getDestAddr() << endl;
					delete macPkt;
				}
			} else {
				EV << "Error! Received an Ack while my send queue was empty. src=" << macPkt->getSrcAddr() << "." << endl;
				delete macPkt;
			}
		}
	}
	else if (dest == L2BROADCAST) {
		executeMac(EV_BROADCAST_RECEIVED, macPkt);
	} else {
		EV << "packet not for me, deleting...\n";
		delete macPkt;
	}
}

void csma::handleLowerControl(cMessage *msg) {
	if (msg->getKind() == MacToPhyInterface::TX_OVER) {
		executeMac(EV_FRAME_TRANSMITTED, msg);
	} else if (msg->getKind() == BaseDecider::PACKET_DROPPED) {
		EV<< "control message: PACKED DROPPED" << endl;
	} else if (msg->getKind() == MacToPhyInterface::RADIO_SWITCHING_OVER) {
		EV<< "control message: RADIO_SWITCHING_OVER to: " << phy->getRadioState() << endl;
	} else {
		EV << "Invalid control message type (type=NOTHING) : name="
		<< msg->getName() << " modulesrc="
		<< msg->getSenderModule()->getFullPath()
		<< "." << endl;
	}
	delete msg;
}

/**
 * Update the internal copies of interesting BB variables
 *
 */
//void csma::receiveBBItem(int category, const BBItem *details, int scopeModuleId) {
//	Enter_Method_Silent();
//	BasicLayer::receiveBBItem(category, details, scopeModuleId);
//
//	if (category == catRadioState) {
//		radioState
//				= static_cast<const RadioAccNoise3State *> (details)->getState();
//		// radio just told us its state
//	} else if (category == catRSSI) {
//		rssi = static_cast<const RSSI *> (details)->getRSSI();
//		if (radioState == RadioAccNoise3State::RX) {
//			// we could do something here if we wanted to.
//		}
//	}
//}

cPacket *csma::decapsMsg(MacPkt * macPkt) {
	cPacket * msg = macPkt->decapsulate();
	MacToNetwControlInfo* info = new MacToNetwControlInfo(macPkt->getSrcAddr());

	msg->setControlInfo(info);
	return msg;
}

