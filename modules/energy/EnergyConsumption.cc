#include "EnergyConsumption.h"

Define_Module(EnergyConsumption);

void EnergyConsumption::initialize(int stage) {
	BaseModule::initialize(stage);
	if (stage == 0) {
		previousTimeStamp 		= 0;
		timeStampsDifference	= 0;

		totalTimeSleep 			= 0;
		totalTimeTx 			= 0;
		totalTimeRx 			= 0;
		totalTimeIdle 			= 0;
		totalTimeCCA 			= 0;
		totalTimeBackOff 		= 0;
		totalTimeRxToSleep 		= 0;
		totalTimeRxToTx 		= 0;
		totalTimeRxToIdle 		= 0;
		totalTimeSleepToRx 		= 0;
		totalTimeSleepToIdle 	= 0;
		totalTimeTxToSleep 		= 0;
		totalTimeTxToRx 		= 0;
		totalTimeTxToIdle		= 0;
		totalTimeIdleToRx 		= 0;
		totalTimeIdleToTx		= 0;
		totalTimeIdleToSleep	= 0;

		totalTimeCalculating 	= 0;
		totalTimeOn 			= 0;

		state 					= RX;
		previousState 			= RX;
		previousMacState 		= MAC_IDLE_1;
		previousPhyState		= RADIO_RX;

		// Time Parameters in seconds
        timeSleepToRx 			= getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeSleepToRX");
        timeSleepToIdle 		= 0.00188;
        timeRxToTx 				= getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeRXToTX");
        timeRxToSleep 			= getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeRXToSleep");
        timeRxToIdle			= 0; // This time MUST BE 0, as the MAC has no transition between CCA and BackOff for example, if we simulate a time, the MAC won't and we have problems with the times
        timeTxToRx 				= getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeTXToRX");
        timeTxToSleep 			= getParentModule()->getSubmodule("nic")->getSubmodule("phy")->par("timeTXToSleep");
        timeTxToIdle			= 0.000192;
        timeIdleToRx			= 0; // This time MUST BE 0, as the MAC has no transition between CCA and BackOff for example, if we simulate a time, the MAC won't and we have problems with the times
        timeIdleToTx			= 0.000192;
        timeIdleToSleep			= 0.00094;

        // Power consumption from boards Parameters in mW/s
        powerUControler			= 0.03042;
        powerSleep 				= 0.0000728;
        powerRx 				= 0.06496;
        powerTx 				= 0.06672;
        powerIdle				= 0.04342;
        powerIdleToTx			= (powerIdle + powerTx) / 2;
        powerIdleToRx			= (powerIdle + powerRx) / 2;
        powerIdleToSleep		= (powerIdle + powerSleep) / 2;
        powerTxToRx				= (powerTx + powerRx) / 2;
        powerTxToSleep			= (powerTx + powerSleep) / 2;
        powerTxToIdle			= (powerTx + powerIdle) / 2;
        powerRxToTx				= (powerRx + powerTx) / 2;
        powerRxToSleep			= (powerRx + powerSleep) / 2;
        powerRxToIdle			= (powerRx + powerIdle) / 2;
        powerSleepToRx			= (powerSleep + powerRx) / 2;
        powerSleepToIdle		= (powerSleep + powerIdle) / 2;
	}
}

void EnergyConsumption::finish() {
	simtime_t totalTime = totalTimeSleep + totalTimeTx + totalTimeRx + totalTimeIdle + totalTimeCCA + totalTimeBackOff + totalTimeRxToSleep + totalTimeRxToTx + totalTimeRxToIdle + totalTimeSleepToRx + totalTimeSleepToIdle + totalTimeTxToSleep + totalTimeTxToRx + totalTimeTxToIdle + totalTimeIdleToRx + totalTimeIdleToTx + totalTimeIdleToSleep;
	simtime_t totalTransitionTime = totalTimeRxToSleep + totalTimeRxToTx + totalTimeRxToIdle + totalTimeSleepToRx + totalTimeSleepToIdle + totalTimeTxToSleep + totalTimeTxToRx + totalTimeTxToIdle + totalTimeIdleToRx + totalTimeIdleToTx + totalTimeIdleToSleep;
	recordScalar("Total Time", totalTime);
	recordScalar("Total Transition time", totalTransitionTime);
	recordScalar("Total Sleep Time", totalTimeSleep);
	recordScalar("Total Tx Time", totalTimeTx);
	recordScalar("Total Rx Time", totalTimeRx);
	recordScalar("Total Idle Time", totalTimeIdle);
	recordScalar("Total CCA Time", totalTimeCCA);
	recordScalar("Total BackOff Time", totalTimeBackOff);
	double totalPowerUControler	= powerUControler * (totalTimeCalculating.dbl() + totalTimeOn.dbl());
	double totalPowerSleep 		= powerSleep * totalTimeSleep.dbl();
	double totalPowerRx 			= powerRx * (totalTimeRx.dbl() + totalTimeCCA.dbl());
	double totalPowerTx 			= powerTx * totalTimeTx.dbl();
	double totalPowerIdle			= powerIdle * (totalTimeIdle.dbl() + totalTimeBackOff.dbl());
	double totalPowerIdleToTx		= powerIdleToTx * totalTimeIdleToTx.dbl();
	double totalPowerIdleToRx		= powerIdleToRx * totalTimeIdleToRx.dbl();
	double totalPowerIdleToSleep	= powerIdleToSleep * totalTimeIdleToSleep.dbl();
	double totalPowerTxToRx		= powerTxToRx * totalTimeTxToRx.dbl();
	double totalPowerTxToSleep		= powerTxToSleep * totalTimeTxToSleep.dbl();
	double totalPowerTxToIdle		= powerTxToIdle * totalTimeTxToIdle.dbl();
	double totalPowerRxToTx		= powerRxToTx * totalTimeRxToTx.dbl();
	double totalPowerRxToSleep		= powerRxToSleep * totalTimeRxToSleep.dbl();
	double totalPowerRxToIdle		= powerRxToIdle * totalTimeRxToIdle.dbl();
	double totalPowerSleepToRx		= powerSleepToRx * totalTimeSleepToRx.dbl();
	double totalPowerSleepToIdle	= powerSleepToIdle * totalTimeSleepToIdle.dbl();
    double totalPower = totalPowerUControler + totalPowerSleep + totalPowerRx +
    		totalPowerTx + totalPowerIdle + totalPowerIdleToTx + totalPowerIdleToRx +
    		totalPowerIdleToSleep + totalPowerTxToRx + totalPowerTxToSleep + totalPowerTxToIdle +
    		totalPowerRxToTx + totalPowerRxToSleep + totalPowerRxToIdle + totalPowerSleepToRx +
    		totalPowerSleepToIdle;
    recordScalar("totalPower", totalPower);
}

void EnergyConsumption::updateStateStatus(bool transmitting, int macState, int phyState) {

	// If the state changed only in PHY layer, than we have to conserve the old MAC State
	if (macState == NUM_MAC_STATES) {
		macState = previousMacState;
	}
	// If the state changed only in MAC layer, than we have to conserve the old PHY State
	if (phyState == NUM_RADIO_STATES) {
		phyState = previousPhyState;
	}

	timeStampsDifference = simTime() - previousTimeStamp;
	EV << timeStampsDifference << endl;

	// Next state calculation, we start the states always before the transition, transition time will be deduced from next state time
	switch (previousState)
	{
	case SLEEP:
		if (phyState == RADIO_RX) {
			if (transmitting) {
				state = IDLE;
			} else {
				state = RX;
			}
		} else if (phyState == RADIO_SLEEP) {
			state = SLEEP;
		}
		//-------------------------------------------------------------------------------------------------------
		//- If we want to transmit directly from sleeping add the case here depending on the MAC state probably -
		//-------------------------------------------------------------------------------------------------------
		totalTimeSleep = totalTimeSleep + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		totalTimeOff = totalTimeOff + timeStampsDifference; // Microcontroller Sleep time
		break;
	case RX:
		if (phyState == RADIO_SLEEP) {
			state = SLEEP;
		} else if ((!transmitting) && (macState == MAC_IDLE_1) && (phyState == RADIO_RX)) {
			state = RX;
		} else if (transmitting) {
			if (((macState == MAC_BACKOFF_2) && (phyState == RADIO_RX)) ||
				((macState == MAC_WAITSIFS_6) && (phyState = RADIO_TX)) ||
				((macState == MAC_IDLE_1) && (phyState == RADIO_RX))) {
					state = IDLE;
			} else if ((macState == MAC_TRANSMITFRAME_4) && (phyState = RADIO_TX)) {
				state = TX;
			}
		}
		if (previousMacState == MAC_CCA_3) {
			totalTimeCCA = totalTimeCCA + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		} else {
			totalTimeRx = totalTimeRx + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		}
		totalTimeOn = totalTimeOn + timeStampsDifference; // Microcontroller On time
		break;
	case TX:
		if (phyState == RADIO_SLEEP) {
			state = SLEEP;
		} else if ((!transmitting) && (macState == MAC_WAITACK_5) && (phyState == RADIO_RX)) {
			state = RX;
		} else if (transmitting) {
			if (((macState == MAC_IDLE_1) && (phyState == RADIO_RX)) ||
				((macState == MAC_BACKOFF_2) && (phyState == RADIO_RX))) {
					state = IDLE;
			}
		}
		totalTimeTx = totalTimeTx + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		totalTimeOn = totalTimeOn + timeStampsDifference; // Microcontroller On time
		break;
	case IDLE:
		if (phyState == RADIO_SLEEP) {
			state = SLEEP;
		} else if ((!transmitting) && (macState == MAC_IDLE_1) && (phyState == RADIO_RX)) {
			state = RX;
		} else if (transmitting) {
			if (((macState == MAC_BACKOFF_2) && (phyState == RADIO_RX)) ||
				((macState == MAC_WAITSIFS_6) && (phyState = RADIO_TX)) ||
				((macState == MAC_IDLE_1) && (phyState == RADIO_RX))) {
					state = IDLE;
			} else if ((macState == MAC_TRANSMITACK_7) && (phyState = RADIO_TX)) {
				state = TX;
			} else if ((macState == MAC_CCA_3) && (phyState == RADIO_RX)) {
				state = RX;
			}
		}
		if (previousMacState == MAC_BACKOFF_2) {
			totalTimeBackOff = totalTimeBackOff + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		} else {
			totalTimeIdle = totalTimeIdle + timeStampsDifference; // Done at the end as till now the state was sleep but in the transittion
		}
		totalTimeOn = totalTimeOn + timeStampsDifference; // Microcontroller On time
		break;
	}

	// Total times calculation
	switch (state)
	{
	case SLEEP:
		if (previousState == RX) {
			previousTimeStamp = simTime() + timeRxToSleep;
			totalTimeRxToSleep = totalTimeRxToSleep + timeRxToSleep;
			totalTimeOnToSleep = totalTimeOnToSleep + timeRxToSleep; // Microcontroller Transition time
		} else if (previousState == TX) {
			previousTimeStamp = simTime() + timeTxToSleep;
			totalTimeTxToSleep = totalTimeTxToSleep + timeTxToSleep;
			totalTimeOnToSleep = totalTimeOnToSleep + timeTxToSleep; // Microcontroller Transition time
		} else if (previousState == IDLE) {
			previousTimeStamp = simTime() + timeIdleToSleep;
			totalTimeIdleToSleep = totalTimeIdleToSleep + timeIdleToSleep;
			totalTimeOnToSleep = totalTimeOnToSleep + timeIdleToSleep; // Microcontroller Transition time
		} else { // SLEEP
			previousTimeStamp = simTime();
		}
		break;
	case RX:
		if (previousState == SLEEP) {
			previousTimeStamp = simTime() + timeSleepToRx;
			totalTimeSleepToRx = totalTimeSleepToRx + timeSleepToRx;
			totalTimeSLeepToOn = totalTimeSLeepToOn + timeSleepToRx; // Microcontroller Transition time
		} else if (previousState == TX) {
			previousTimeStamp = simTime() + timeTxToRx;
			totalTimeTxToRx = totalTimeTxToRx + timeTxToRx;
			totalTimeOn = totalTimeOn + timeTxToRx; // Microcontroller On time
		} else if (previousState == IDLE) {
			previousTimeStamp = simTime() + timeIdleToRx;
			totalTimeIdleToRx = totalTimeIdleToRx + timeIdleToRx;
			totalTimeOn = totalTimeOn + timeIdleToRx; // Microcontroller On time
		} else { // RX
			previousTimeStamp = simTime();
		}
		break;
	case TX:
		if (previousState == SLEEP) {
			// Case not contemplated
		} else if (previousState == RX) {
			previousTimeStamp = simTime() + timeRxToTx;
			totalTimeRxToTx = totalTimeRxToTx + timeRxToTx;
			totalTimeOn = totalTimeOn + timeRxToTx; // Microcontroller On time
		} else if (previousState == IDLE) {
			previousTimeStamp = simTime() + timeIdleToTx;
			totalTimeIdleToTx = totalTimeIdleToTx + timeIdleToTx;
			totalTimeOn = totalTimeOn + timeIdleToTx; // Microcontroller On time
		} else { // TX
			previousTimeStamp = simTime();
		}
		break;
	case IDLE:
		if (previousState == SLEEP) {
			previousTimeStamp = simTime() + timeSleepToIdle;
			totalTimeSleepToIdle = totalTimeSleepToIdle + timeSleepToIdle;
			totalTimeSLeepToOn = totalTimeSLeepToOn + timeSleepToIdle; // Microcontroller Transition time
		} else if (previousState == TX) {
			previousTimeStamp = simTime() + timeTxToIdle;
			totalTimeTxToIdle = totalTimeTxToIdle + timeTxToIdle;
			totalTimeOn = totalTimeOn + timeTxToIdle; // Microcontroller On time
		} else if (previousState == RX) {
			previousTimeStamp = simTime() + timeRxToIdle;
			totalTimeRxToIdle = totalTimeRxToIdle + timeRxToIdle;
			totalTimeOn = totalTimeOn + timeRxToIdle; // Microcontroller On time
		} else { // IDLE
			previousTimeStamp = simTime();
		}
		break;
	}

	previousMacState = macState;
	previousPhyState = phyState;
	previousState = state;

	switch (state)
	{
	case RX:
		EV << "State: RX" << endl;
		break;
	case TX:
		EV << "State: TX" << endl;
		break;
	case SLEEP:
		EV << "State: SLEEP" << endl;
		break;
	case IDLE:
		EV << "State: IDLE" << endl;
		break;
	}
	simtime_t totalTime = totalTimeSleep + totalTimeTx + totalTimeRx + totalTimeIdle + totalTimeCCA + totalTimeBackOff + totalTimeRxToSleep + totalTimeRxToTx + totalTimeRxToIdle + totalTimeSleepToRx + totalTimeSleepToIdle + totalTimeTxToSleep + totalTimeTxToRx + totalTimeTxToIdle + totalTimeIdleToRx + totalTimeIdleToTx + totalTimeIdleToSleep;
	simtime_t totalTransitionTime = totalTimeRxToSleep + totalTimeRxToTx + totalTimeRxToIdle + totalTimeSleepToRx + totalTimeSleepToIdle + totalTimeTxToSleep + totalTimeTxToRx + totalTimeTxToIdle + totalTimeIdleToRx + totalTimeIdleToTx + totalTimeIdleToSleep;
	EV << "Total Time: " << totalTime << endl;
	EV << "Total Transition time: " << totalTransitionTime << endl;
	EV << "Total Sleep Time: " << totalTimeSleep << endl;
	EV << "Total Tx Time: " << totalTimeTx << endl;
	EV << "Total Rx Time: " << totalTimeRx << endl;
	EV << "Total Idle Time: " << totalTimeIdle << endl;
	EV << "Total CCA Time: " << totalTimeCCA << endl;
	EV << "Total BackOff Time: " << totalTimeBackOff << endl;
	EV << "---------------------------------" << endl;
	EV << "Total uC Sleep Time: " << totalTimeOff << endl;
	EV << "Total uC On Time: " << totalTimeOn << endl;
	EV << "Total uC Transition time: " << totalTimeOnToSleep + totalTimeSLeepToOn << endl;
	EV << "Total uC Calculatin time: " << totalTimeCalculating << endl;
}

void EnergyConsumption::updateCalculatingTime(simtime_t time)
{
	totalTimeCalculating = totalTimeCalculating + time;
	// The time we add here at the end of the calculating phase, we have to take it out from one of this two places
	if (state == SLEEP) { // After the Sync Phase 1 we went to sleep
		totalTimeOnToSleep = totalTimeOnToSleep - time; // We have to take out this time as just before calculating we are always listening to the Sync Phase 1, and time < timeRxToSleep
	} else { // After the Sync Phase 1 we went into idle because a report transmission comes soon
		totalTimeOn = totalTimeOn - time;
	}
}
