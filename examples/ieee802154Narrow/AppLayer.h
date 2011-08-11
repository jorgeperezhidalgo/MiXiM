#ifndef APPLAYER_H_
#define APPLAYER_H_

#include "BaseLayer.h"
#include "ApplPkt_m.h"
#include "SimpleAddress.h"
#include "BaseArp.h"
#include <BaseWorldUtility.h>
#include "BaseConnectionManager.h"
#include "NetwControlInfo.h"
#include "AppToNetControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>
#include <omnetpp.h>

class AppLayer : public BaseLayer
{
public:

	enum TrafficGenMessageKinds{		// All the possible types of messages/tasks to send/schedule

		SEND_SYNC_TIMER_WITH_CSMA = 1,	// Used to schedule the next message
		SYNC_MESSAGE_WITH_CSMA,			// Used by the Mobile Nodes type 3 and 4 to send their broadcasts
		SEND_SYNC_TIMER_WITHOUT_CSMA,	// Used to schedule the next message
		SYNC_MESSAGE_WITHOUT_CSMA,		// Used by the Anchors to send the sync messages
		SEND_REPORT_WITH_CSMA,			// Used to schedule the next message
		REPORT_WITH_CSMA,				// Report from any host with CSMA enabled
		SEND_REPORT_WITHOUT_CSMA,		// Used to schedule the next message
		REPORT_WITHOUT_CSMA,			// Report from any host with CSMA disabled
		CALCULATE_POSITION,				// Event to simulate the processing time of the Mobile Node type 2 when it calculates its position
		CHECK_QUEUE,					// Event to check the transmission queues from Anchors (Com Sink 1) and Computer (Com Sink 2)
		WAITING_REQUEST,				// Event to simulate the waiting time since we send the request till we receive the answer from the Anchor
		BEGIN_PHASE,					// Event to be executed at the beginning of every phase
		WAKE_UP,						// Event to wake up the node timeSleepToRX before activity
		SLEEP							// Event to sleep the node
	};

	enum PhaseType{						// Phases of the Full Phase or Period

		SYNC_PHASE_1 = 1,
		REPORT_PHASE ,
		VIP_PHASE,
		SYNC_PHASE_2,
		COM_SINK_PHASE_1,
		SYNC_PHASE_3,
		COM_SINK_PHASE_2
	};


protected:

	BaseArp* arp;						// Pointer to the ARP of the Host
	int myNetwAddr;						// Network address of the Host

	BaseConnectionManager* cc;			// Pointer to the Propagation Model module

	// All of them except syncPacket have a + 6 bytes due to the extra bytes needed by the long address for Mobile Nodes, it corresponds to MAC layer but here is easier to add the offset
   	int syncPacketLength;				// Sync Packet Length, includes 1 byte status, 4 byte timestamp and 2 + 2 + 2 bytes possition
   	int normalReportPacketLength;		// Rutinary Report Packet Length, includes 1 byte status and should be added 2 bytes x each listened Anchor, only exists in MN type 1 and 4
   	int type2ReportPacketLength;		// MN type 2 Report Packet Length, includes 1 byte status and should be added (2 + 2 + 2 = 6) position bytes x each position calculated, only MN type 2
   	int askReportPacketLength;			// Ask Report Packet Length, includes 1 byte status and 10 bytes information. This is the report to ask for information to the AN in next period
   	int requestPacketLength;			// Request Packet Length, includes 1 byte status and 1 aditional byte information. This request asks for the theoretically already available information in AN
   	int broadcastPacketLength;			// Broadcast Packet Length, includes 1 byte status and 1 aditional byte information. This is the broadcast packet sent by MN type 3 and 4
   	int answerANtoMNPacketLength;		// Report sent by AN to MN when MN makes a request, includes 1 byte status and 10 bytes info.
   	int bcastMixANPacketLength;			// Report sent by AN when it receives broadcasts from a MN, sends a report per broadcast, includes 1 byte status and 2 bytes RSSI info.

	long destination;					// Broadcast destination, usually -1

	int syncPacketsPerSyncPhase; 		// Determines how many times do we repeat all the slots per sync phase

	simtime_t syncPacketTime;			// Max. duration of a Sync Packet, determines the slot size
	simtime_t fullPhaseTime;			// Duration of the Full Phase or Period
	simtime_t timeComSinkPhase;			// Duration of every Com Sink Phase
	simtime_t timeSyncPhase;			// Duration of every Sync Phase, everyone is formed by syncPacketsPerSyncPhase mini sync phases
	simtime_t timeReportPhase;			// Duration of the Report Phase
	simtime_t timeVIPPhase;				// Duration of the VIP Phase
	double phase2VIPPercentage;			// Percentage of the time Phase Report + Phase VIP that the Phase VIP takes

	simtime_t guardTimeReportPhase;		// Guard time to leave at the end of the Report Phase, so the transmissions don't invade next phase
	simtime_t guardTimeVIPPhase;		// Guard time to leave at the end of the VIP Phase, so the transmissions don't invade next phase
	simtime_t guardTimeComSinkPhase;	// Guard time to leave at the end of every Com Sink Phase, so the transmissions don't invade next phase
	simtime_t smallTime;				// Time to add to another time when we want to make an event after another when they should execute at the same time
	simtime_t nextPhaseStartTime;		// Time to know the next Phase Start Time

	int numTotalSlots;					// Number of Slots in every mini sync Phase, they are calculated by the computer in the initialize method
	int numberOfAnchors;				// Number of Anchors in the Network
	int numberOfNodes;					// Number of Mobile Nodes in the Network

	long nbPacketDroppedNoACK;			// Variable to count the number of packets that were dropped when sending a Report from a Mobile Node and don't getting ACK
	long nbPacketDroppedBackOff;		// Variable to count the number of packets that were dropped when sending a Report and reaching maximum BackOff number
	long nbPacketDroppedAppQueueFull;	// Variable to count the number of dropped packets because the App queue is full
	long nbPacketDroppedMacQueueFull;	// Variable to count the number of dropped packets because the Mac queue is full
	long nbPacketDroppedNoTimeApp;		// Variable to count the number of packets that were dropped at application layer when there was no more time for an AN to transmit more
	long nbAnswersRequestOutOfTime;		// Variable to count the number of packets that arrived to the mobile node when the waiting time was over
	long nbRequestWihoutAnswer;			// Variable to count the number of requests from the Mobile Node that didn't get any answer from an Anchor
	long nbErasedPacketsBackOffMax;		// Variable to count the number of definitely erased packets due to maximum Backoff retransmissions in App Layer
	long nbErasedPacketsNoACKMax;		// Variable to count the number of definitely erased packets due to maximum No ACK received retransmissions in App Layer
	long nbErasedPacketsMacQueueFull;	// Variable to count the number of definitely erased packets due to maximum Mac Queue Full

	long nbReportsGeneratedDueToBcast;	// Variable to count the number of reports an AN sends due to the broadcasts received from MNs
	long nbReportsResent;				// Variable to count the number or reports received in an AN resent to computer
	long nbReportsRoutedCS1;			// Variable to count the number of reports routed from an AN when received from another AN in Com Sink 1
	long nbReportsRoutedCS2;			// Variable to count the number of reports routed from an AN when received from another AN in Com Sink 2

	long nbBroadcastPacketsSent;		// Variable to count the number of broadcast packets successfully sent in the air, broadcast for MN or sync for AN
	long nbReportsWithACK;				// Variable to count the number of Reports successfully sent -> ACK received
	long nbBroadcastPacketsReceived;	// Variable to count the number of broadcast packets successfully received
	long nbReportsReceived;				// Variable to count the number of reports successfully received
	long nbReportsForMeReceived;		// Variable to count the number of reports that were really for me and I received

	NicEntry* computer;					// Pointer to the NIC of the computer to take general data over the configurations
	NicEntry* host;						// Pointer to a host, we use it to point the module who sent the message to check what type of module is (AN, MN, Comp)

	int maxRetransDroppedReportMN;		// Maximum number of retransmissions from the App Layer when a Mobile Node gets a drop from MAC when it sends a Report
	int maxRetransDroppedReportAN;		// Maximum number of retransmissions from the App Layer when an Anchor gets a drop from MAC when it sends a Report
	int maxRetransDroppedBackOff;		// Maximum number of retransmissions from the App Layer when we get a CAF (Drop because of too many backoffs) from the MAC

	cQueue transfersQueue;				// FIFO to store the packets that we sent until we receive a confirmation from the MAC that they were sent or until we receive a drop and then try again
	cMessage * beginPhases;				// Event to drop all the elements in the queue at the beginning from every phase
	PhaseType phase;					// To know which phase we are in
	PhaseType nextPhase;				// To know which phase is the next


public:

	virtual void initialize(int stage);
	virtual int numInitStages() const {return 5;}

};

#endif
