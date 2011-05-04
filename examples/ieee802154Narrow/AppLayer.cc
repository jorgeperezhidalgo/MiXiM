#include "AppLayer.h"
#include "NetwControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>


void AppLayer::initialize(int stage)
{
	BaseLayer::initialize(stage);

	if(stage == 0) {
		arp = FindModule<BaseArp*>::findSubModule(findHost());
		myNetwAddr = arp->myNetwAddr(this);

        cc = FindModule<BaseConnectionManager *>::findGlobalModule();
        if( cc == 0 ) error("Could not find connectionmanager module");

        // Parameter load
		syncPacketsPerSyncPhase = par("syncPacketsPerSyncPhase");
		if (syncPacketsPerSyncPhase <= 0) error("You must introduce a positive number of periods.");

		packetLength = par("packetLength");
		packetTime = par("packetTime");
		destination = par("destination");

		syncPacketTime = par("syncPacketTime");
		phase2VIPPercentage = par("phase2VIPPercentage");
		fullPhaseTime = getParentModule()->getParentModule()->par("fullPhaseTime");
		timeComSinkPhase = getParentModule()->getParentModule()->par("timeComSinkPhase");
		numberOfAnchors = getParentModule()->getParentModule()->par("numAnchors");
		numberOfNodes = getParentModule()->getParentModule()->par("numNodes");

		// Variable initialization, we could change this into parameters if necesary
		nbPacketDroppedReportMN = 0;
		nbPacketDroppedReportAN = 0;
		nbPacketDroppedBackOffMN = 0;
		nbPacketDroppedBackOffAN = 0;
		nbPacketDroppedNoTimeAN = 0;
		nbPacketDroppedNoTimeMN = 0;
		numOfPcktsRcvdInWrongPhase = 0;
		nbRqstWihoutAnswer = 0;
		guardTimeReportPhase = 0.020; 	// 20 ms
		guardTimeVIPPhase = 0.020; 		// 20 ms
		guardTimeComSinkPhase = 0.080; 	// 80 ms
		maxRetransDroppedReportMN = 1;
		maxRetransDroppedReportAN = 2;
		maxRetransDroppedBackOff = 3;
	} else if (stage == 4) { // We have to wait till stage 4 because before the number of slots are not yet calculated
		computer = cc->findNic(getParentModule()->getParentModule()->getSubmodule("computer", 0)->findSubmodule("nic"));
		timeSyncPhase = syncPacketsPerSyncPhase * computer->numTotalSlots * syncPacketTime ;
		timeVIPPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * timeSyncPhase)) * phase2VIPPercentage;
		timeReportPhase = (fullPhaseTime - (2 * timeComSinkPhase) - (3 * timeSyncPhase)) * (1 - phase2VIPPercentage);
	}
}

AppLayer::PhaseType AppLayer::InWhichPhaseAmI(simtime_t fullPhaseTime, simtime_t timeSyncPhase, simtime_t timeReportPhase, simtime_t timeVIPPhase, simtime_t timeComSinkPhase)
{
	// All the times are in seconds
	double timeNow = simTime().dbl() + 0.000001;

	double phasePoint = fmod(timeNow, fullPhaseTime.dbl());

	if (phasePoint < timeSyncPhase.dbl())
		return AppLayer::SYNC_PHASE_1;
	else if (phasePoint < (timeSyncPhase.dbl() + timeReportPhase.dbl()))
		return AppLayer::REPORT_PHASE;
	else if (phasePoint < (timeSyncPhase.dbl() + timeReportPhase.dbl() + timeVIPPhase.dbl()))
		return AppLayer::VIP_PHASE;
	else if (phasePoint < (timeSyncPhase.dbl() + timeReportPhase.dbl() + timeVIPPhase.dbl() + timeSyncPhase.dbl()))
		return AppLayer::SYNC_PHASE_2;
	else if (phasePoint < (timeSyncPhase.dbl() + timeReportPhase.dbl() + timeVIPPhase.dbl() + timeSyncPhase.dbl() + timeComSinkPhase.dbl()))
		return AppLayer::COM_SINK_PHASE_1;
	else if (phasePoint < (timeSyncPhase.dbl() + timeReportPhase.dbl() + timeVIPPhase.dbl() + timeSyncPhase.dbl() + timeComSinkPhase.dbl() + timeSyncPhase.dbl()))
		return AppLayer::SYNC_PHASE_3;
	else
		return AppLayer::COM_SINK_PHASE_2;
}
