#ifndef ENERGYCONSUMPTION_H_
#define ENERGYCONSUMPTION_H_

#include <BaseModule.h>

class EnergyConsumption : public BaseModule
{
public:

	// Variables to use in this class
	simtime_t previousTimeStamp;			// Variable to store the previous time stamp to take the time difference with the new one
	simtime_t timeStampsDifference;			// Variable to store the time difference between the previous time stamp and the current one

	// Transceiver states total times
	simtime_t totalTimeSleep;				// Variable to store the total time this module spent in Sleep Mode
	simtime_t totalTimeTx;					// Variable to store the total time this module spent in Tx Mode
	simtime_t totalTimeRx;					// Variable to store the total time this module spent in Rx Mode but out of CCA
	simtime_t totalTimeIdle;				// Variable to store the total time this module spent in Idle Mode but out of BackOff
	simtime_t totalTimeCCA;					// Variable to store the total time this module spent doing CCA
	simtime_t totalTimeBackOff;				// Variable to store the total time this module spent in BackOff waits
	simtime_t totalTimeRxToSleep;			// Variable to store the total time this module spent in transition Rx to Sleep (after of ACK, end of sync phase)
	simtime_t totalTimeRxToTx;				// Variable to store the total time this module spent in transition Rx to Tx (After CCA if channel free to transmit, receiving report and then send ACK)
	simtime_t totalTimeRxToIdle;			// Variable to store the total time this module spent in transition Rx to Idle (After CCA if channel busy again backoff)
	simtime_t totalTimeSleepToRx;			// Variable to store the total time this module spent in transition Sleep to Rx (Wake up phy to listen a sync phase)
	simtime_t totalTimeSleepToIdle;			// Variable to store the total time this module spent in transition Sleep to Idle (When we wake up phy to transmit)
//	simtime_t totalTimeSleepToTx;			// Not used yet, before transmitting always listen
	simtime_t totalTimeTxToSleep;			// Variable to store the total time this module spent in transition Tx to Sleep (After transmitting a Broadcast)
	simtime_t totalTimeTxToRx;				// Variable to store the total time this module spent in transition Tx to Rx (After transmitting report wait for the ACK)
	simtime_t totalTimeTxToIdle;			// Variable to store the total time this module spent in transition Tx to Idle (After transmitting an ACK or broadcast when no waiting for ACK, but no need to sleep as we will activity soon)
	simtime_t totalTimeIdleToRx;			// Variable to store the total time this module spent in transition Idle to Rx (After backoff -> CCA)
	simtime_t totalTimeIdleToTx;			// Variable to store the total time this module spent in transition Idle to Tx (After receiving a packet and before transmitting ACK we wait for SIFS -> Idle)
	simtime_t totalTimeIdleToSleep;			// Variable to store the total time this module spent in transition Idle to Sleep

	// There could be also timeIdleToSleep, timeIdleToTx, timeTxToIdle and timeTxToSleep, but in our case doesn't work with them, if needed just create variable and increase them in the correct state from state machine

	// App states time
	simtime_t totalTimeCalculating;			// Variable to store the total time this module spent in calculating position in Mobile Node type 2
	simtime_t totalTimeOn;					// Variable to store the total time this module spent on out of calculating
	simtime_t totalTimeOnToSleep;			// Variable to store the total time this module spent in transitions between on and sleep
	simtime_t totalTimeSLeepToOn;			// Variable to store the total time this module spent in transitions between sleep and on
	simtime_t totalTimeOff;					// Variable to store the total time this module spent sleeping the micro controller

	// Parameter transition times for the used chip
	simtime_t timeSleepToRx;
//	simtime_t timeSleepToTx; // No need yet for this time
	simtime_t timeSleepToIdle;
	simtime_t timeRxToTx;
	simtime_t timeRxToSleep;
	simtime_t timeRxToIdle;
	simtime_t timeTxToRx;
	simtime_t timeTxToSleep;
	simtime_t timeTxToIdle;
	simtime_t timeIdleToRx;
	simtime_t timeIdleToTx;
	simtime_t timeIdleToSleep;

    // Power consumption from boards Parameters in mW/s
	double powerUControler; //µControler Power
    double powerSleep;
    double powerRx;
    double powerTx;
    double powerIdle;
    double powerIdleToTx;
    double powerIdleToRx;
    double powerIdleToSleep;
    double powerTxToRx;
    double powerTxToSleep;
    double powerTxToIdle;
    double powerRxToTx;
    double powerRxToSleep;
    double powerRxToIdle;
    double powerSleepToRx;
//    double powerSleepToTx; // No need yet for this power
    double powerSleepToIdle;


	// States for state machine
	enum States{						// Phases of the Full Phase or Period

		SLEEP = 0,
		IDLE,
		RX,
		TX
	};

    /** @brief MAC states
     * see states diagram.
     */
    enum t_mac_states {
		MAC_IDLE_1=1,
		MAC_BACKOFF_2,
		MAC_CCA_3,
		MAC_TRANSMITFRAME_4,
		MAC_WAITACK_5,
		MAC_WAITSIFS_6,
		MAC_TRANSMITACK_7,
		// Just to count the number of states
		NUM_MAC_STATES

    };

	/**
	* @brief The state of the radio of the nic.
	*/
	enum RadioState {
		RADIO_RX = 0,
		RADIO_TX,
		RADIO_SLEEP,
		RADIO_SWITCHING,
		// Just to count the number of states
		NUM_RADIO_STATES
	};

	States state;							// Variable to store the actual state
	States previousState;					// Variable to store the previous state
	int previousMacState;	// Variable to store the previous MAC state to have the value when there is no change
	int previousPhyState;		// Variable to store the previous PHY state to have the value when there is no change

public:

    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);
    virtual void finish();

    // Function to update the state machine and time counters depending on the MAC state, PHY state and if we are going to liste or transmit
    void updateStateStatus(bool transmitting, int macState, int phyState);

    // Function that adds the calculating time taking it out from the sleep and transition time
    void updateCalculatingTime(simtime_t time); // The time is the time this calculating time takes

};

#endif
