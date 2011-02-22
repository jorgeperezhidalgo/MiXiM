//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef ANCHORAPPLAYER_H_
#define ANCHORAPPLAYER_H_

#include "SyncPkt_m.h"
#include "SimpleAddress.h"
#include "BaseLayer.h"
#include "BaseArp.h"
#include <BaseWorldUtility.h>
#include "BaseConnectionManager.h"


#include <omnetpp.h>

/**
 * @brief A module to generate traffic for the NIC, used for testing purposes.
 *
 * @ingroup exampleIEEE802154Narrow
 */
class AnchorAppLayer : public BaseLayer
{
public:

	enum TrafficGenMessageKinds{

		SEND_SYNC_TIMER = 1,
		SYNC_MESSAGE
	};

protected:

	int packetLength;
	simtime_t packetTime;
	int phaseRepetitionNumber;
	int syncPacketsPerSyncPhase; // Determines how many times do we repeat all the slots per sync phase
	int syncPacketsPerSyncPhaseCounter; // In which of the repetitions are we already
	int syncPhaseNumber; //To identify in which of the 3 sync phases are we inside the fullphase
	int numSlots;
	long destination;
	int anchorType;
	int numAnchors;
	simtime_t syncPacketTime;
	simtime_t fullPhaseTime;
	simtime_t timeComSinkPhase;
	simtime_t timeSyncPhase;
	simtime_t timeReportPhase;
	simtime_t timeVIPPhase;
	double phase2VIPPercentage;
	int scheduledSlot; //When a node has assigned more than 1 slot, we have to create a new entry in the same sync phase
	simtime_t lastPhaseStart;
	simtime_t nextSyncSend;
	NicEntry* anchor;

	bool syncInSlot; // Indicates if the sync packets have to be slotted or not
   	simtime_t syncFirstMaxRandomTime; // First maximum time an anchor must wait to transmit the first sync packet in no slotted mode
   	simtime_t syncRestMaxRandomTimes; // Rest of maximum times an anchor must wait to transmit the rest of the sync packets in no slotted mode

	int catPacket;

	long nbPacketDropped;


	BaseArp* arp;
	int myNetwAddr;

	cMessage *delayTimer;

	BaseWorldUtility* world;

	/** @brief Pointer to the PropagationModel module*/
	BaseConnectionManager* cc;

public:
	virtual ~AnchorAppLayer();

	virtual void initialize(int stage);

	virtual int numInitStages() const {return 5;}

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

	// Returns from the already assigned slots how many time a nicId appears
	int hasSlot(int *slots, int *slotCounter, int numSlots, int anchor, int numAnchors);

	// Order Queue min to max
	void orderQueue(int *queue, int *numerTimesQueue, int queueCounter);

};

#endif
