#ifndef NODEAPPLAYER_H_
#define NODEAPPLAYER_H_

#include "AppLayer.h"

class NodeAppLayer : public AppLayer
{
public:

	enum MobileNodeType {				// Defines the four possible types of Mobile Nodes

		LISTEN_AND_REPORT = 1,
		LISTEN_AND_CALCULATE,
		VIP_TRANSMIT,
		LISTEN_TRANSMIT_REPORT
	};

	typedef struct {					// Struct to store the RSSI info, we add all of them and then at the end we divide to get the mean, each position is from an AN 0, 1, 2 ... 24
		double RSSIAdition;
		int counterRSSI;
	} RSSIs;

protected:

	int nodeConfig;						// Which of the four types is the MN, check the MobileNodeType to see the possible types

	int offsetPhases; 					// Number of Offset full phases to start the first active full phase (to avoid that all MN transmit at the same full phase)
	int offsetSyncPhases;				// Number of sync phases we don't read at the beggining of the first active phase, it can be 0, 1 or 2, with 3 we would set an active phase less
	int activePhases; 					// Configuration number of active phases, where we perform the standard functions of every type
	int activePhasesCounter; 			// Counter to know when do we reach the maximum active full phases
	int inactivePhases; 				// Configuration number of inactive phases, where we do nothing or extra reports if scheduled
	bool activePhase; 					// To sign if we are in an active phase or not
	bool syncListen;					// If true we will listen to the sync phase, if false we won't take any RSSI value

	int offsetReportPhases; 			// Number of Offset full phases to start the first extra report full phase (to avoid that all MN transmit at the same full phase)
	int reportPhases; 					// Configuration number to know every how many full phases do we make an extra report
	bool makeExtraReport; 				// True we make it (listen sync and report), false we don't (just listen sync, for synchronization just)
	bool syncListenReport;				// If true we will listen to the sync phase, if false we won't take any RSSI value (for the extra report)
	int askFrequency;					// Every askFrequency Extra Reports we activate the ASK flag to make a request
	int askFrequencyCounter;			// Counter to know when do we reach the askFrequency and mark the packet with askForRequest Flag
	bool askForRequest;					// This FLAG in the packet tells the AN that in the next full phase (period) the Mobile Node will ask for some information and the AN has to have it ready
	bool requestPacket;					// This FLAG in the packet tells the AN that it has to answer the Mobile Node with some info and if it doesn't have anything ask anyway saying the info is not yet there
	int waitForAnchor;					// This value will be set to the MAC of the AN we are waiting the packet from when we make a request
	int anchorDestinationRequest;		// Here we would save the MAC address of the Anchor where we send the report to have it when we send the request in case we delete the RSSI values

	long nbPacketRequestedFailed;		// Variable to count the number of packets that were requested but never arrived in the receiving time

	int numberOfBroadcasts; 			// Number of broadcasts (VIP or normal) done by a MN type 3 or 4
	int numberOfBroadcastsCounter; 		// Counter to know when we reach the numberOfBroadcasts
	simtime_t* randomTransTimes; 		// Vector to store the times when the broadcasts are transmitted, it is calculated before
	simtime_t minimumSeparation; 		// Minimum separation between broadcasts to be able to transmit it without problems
	bool minimumDistanceFulfilled; 		// Boolean to check if the minimum distance is fulfilled
	simtime_t type4ReportTime; 			// Time when the report for MN type 4 is scheduled, to take into account to distribute the broadcasts with a minimum distance

	int positionsToSave; 				// Number of positions to save for the MN in Type 2
	Coord* positionFIFO; 				// Vector where we save the last positions from this Mobile Node
	int positionsSavedCounter; 			// Counter where we save the number of elements of the array
	bool isProcessing; 					// To check if we are processing a position in Type 2, during this time we cannot send a Report
	simtime_t processingTime;			// Time we are processing the packets to calculate the position while we cannot transmit
	simtime_t endProcessingTime;		// End of this time, used to schedule an event to indicate that we already can send packets
	simtime_t waitForRequestTime;		// Maximum time we wait till we get the answer from AN to our request

	bool centralized; 					// In Type 1, who calculates the position? True = Computer, False = AN Selected

	RSSIs* listRSSI;					// Vector where we save all the RSSI values read from the sync packets in sync phase
	int indiceRSSI;						// Used to get the index of the AN or computer where we get the RSSI from to use it as index for the previous vector (computer = numberOfAnchors)

	cMessage *configureFullPhase;		// Configuration message
	cMessage *configureExtraReport;		// Configuration extra report
	cMessage *sendReportWithCSMA;		// Send Report message
	cMessage *sendExtraReportWithCSMA;	// Send Extra report message
	cMessage *sendSyncTimerWithCSMA;	// Broadcast messages for Type 3 and 4
	cMessage *calculatePosition;		// Calculate Node Position
	cMessage *waitForRequest;			// Waiting for Request Answer

	//BORRAR CUANDO ESTÉ TODO PROBADO
	cMessage *PUTEAR;

	NicEntry* node;						// Pointer to the NIC of this anchor to access some NIC variables

public:
	virtual ~NodeAppLayer();

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
		opp_error("NodeAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Handle control messages from upper layer */
	virtual void handleUpperControl(cMessage *msg)
	{
		opp_error("NodeAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Send a broadcast message to lower layer. */
	virtual void sendBroadcast();

	// Send a report to the lower layer
	void sendReport();

	// Operations for our FIFO where we stor the positions calculated in mobile node type 2
	void insertElementInFIFO(Coord position);
	void insertElementInFIFO(double x, double y);
	Coord extractElementFIFO();

	// Operations to create and order our random transmission times vector for the broadcasts in type 3 and 4
	void createRandomBroadcastTimes(simtime_t maxTime);
	void orderVectorMinToMax(simtime_t* randomTransTimes, int numberOfBroadcasts);
};

#endif
