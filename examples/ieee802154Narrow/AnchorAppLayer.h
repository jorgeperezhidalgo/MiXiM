#ifndef ANCHORAPPLAYER_H_
#define ANCHORAPPLAYER_H_

#include "AppLayer.h"

class AnchorAppLayer : public AppLayer
{
protected:

	int phaseRepetitionNumber;			// Total number of full phases, -1 = unlimited
	bool syncInSlot; 					// Indicates if the sync packets have to be slotted or not

	int syncPacketsPerSyncPhaseCounter; // In which of the repetitions (mini sync phases) are we already in the sync phase now
	PhaseType syncPhaseNumber; 			// To identify in which of the 3 sync phases are we inside the full phase
	int scheduledSlot; 					// When a node has assigned more than 1 slot, we have to create a new entry in the same sync phase, this variable count how many slots did we already assign

	simtime_t nextPhaseStart;			// Start time for the next slot period
	simtime_t nextSyncSend;				//
   	simtime_t syncFirstMaxRandomTime; 	// First maximum time an anchor must wait to transmit the first sync packet in no slotted mode
   	simtime_t syncRestMaxRandomTimes; 	// Rest of maximum times an anchor must wait to transmit the rest of the sync packets in no slotted mode

	cQueue packetsQueue;				// FIFO to store the packets we receive in Report and VIP phases to send them to Comptuer in Com Sink 1
	simtime_t *randomQueueTime;			// Vector of random times to transmit the queue along the Com Sink 1
	simtime_t stepTimeComSink1;			// Step time in which we divide the Com Sink 1 Phase - The guard time. We divide it in so many parts like elements in the queue
	int queueElementCounter;			// Variable to know how many queue elements have we already transmitted, therefore to calculate all the random transmitting times when = 0 or knowing which randomQueueTime is the next to use
	int maxQueueElements;				// Maximum number of packets in the queue to transmit in the next Com Sink 2 Phase

	int *broadcastCounter;				// Vector of number of broadcasts from Mobile Node already read, to check if we just read the last one
	int indexBroadcast;					// Number of Mobile Node who sent the broadcast to store the value of Broadcast received in the correct field

	cMessage *delayTimer;				// Pointer to the event we use to schedule all the sync packets in the sync phases
	cMessage *checkQueue;				// Variable to schedule the events to process the Queue elements

	NicEntry* anchor;					// Pointer to the NIC of this anchor to access some NIC variables

public:
	virtual ~AnchorAppLayer();

	virtual void initialize(int stage);

	virtual void finish();

protected:
	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);

	/** @brief Handle messages from lower layer */
	virtual void handleLowerMsg(cMessage *msg);

	/** @brief Handle control messages from lower layer */
	virtual void handleLowerControl(cMessage *msg);

	/** @brief Handle messages from upper layer */
	virtual void handleUpperMsg(cMessage *msg)
	{
		opp_error("AnchorAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Handle control messages from upper layer */
	virtual void handleUpperControl(cMessage *msg)
	{
		opp_error("AnchorAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Send a broadcast message to lower layer. */
	virtual void sendBroadcast();

};

#endif
