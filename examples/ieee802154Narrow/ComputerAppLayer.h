#ifndef ANCHORAPPLAYER_H_
#define ANCHORAPPLAYER_H_

#include "AppLayer.h"

class ComputerAppLayer : public AppLayer
{
protected:

	cQueue packetsQueue;			// FIFO to store the packets we receive in Com Sink 1 to send them back in Com Sink 2
	simtime_t startTimeComSink2;	// Variable to store the time start of Com Sink 2 to schedule new queue processing
	simtime_t *randomQueueTime;		// Vector of random times to transmit the queue along the Com Sink 2
	simtime_t stepTimeComSink2;		// Step time in which we divide the Com Sink 2 Phase - The guard time. We divide it in so many parts like elements in the queue
	int queueElementCounter;		// Variable to know how many queue elements have we already transmitted, therefore to calculate all the random transmitting times when = 0 or knowing which randomQueueTime is the next to use

	cMessage *checkQueue;			// Variable to schedule the events to process the Queue elements

public:
	virtual ~ComputerAppLayer();

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
		opp_error("ComputerAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Handle control messages from upper layer */
	virtual void handleUpperControl(cMessage *msg)
	{
		opp_error("ComputerAppLayer has no upper layers!");
		delete msg;
	}

	// Returns from the already assigned slots how many times a nicId appears
	int hasSlot(int *slots, int *slotCounter, int numSlots, int anchor, int numAnchors);

	// Order Queue min to max in number of slots assigned (numberTimesQueue)
	void orderQueue(int *queue, int *numerTimesQueue, int queueCounter);
};

#endif
