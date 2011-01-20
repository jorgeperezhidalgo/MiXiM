/* -*- mode:c++ -*- ********************************************************
 * file:        PhyLayerUWBIR.h
 *
 * author:      Jerome Rousselot <jerome.rousselot@csem.ch>
 *
 * copyright:   (C) 2008 Centre Suisse d'Electronique et Microtechnique (CSEM) SA
 * 				Systems Engineering
 *              Wireless Embedded Systems
 *              Jaquet-Droz 1, CH-2002 Neuchatel, Switzerland.
 *
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 * description: this physical layer models an ultra wideband impulse radio channel.
 * acknowledgment: this work was supported (in part) by the National Competence
 * 			    Center in Research on Mobile Information and Communication Systems
 * 				NCCR-MICS, a center supported by the Swiss National Science
 * 				Foundation under grant number 5005-67322.
 ***************************************************************************/
//
// See the following publications for more information:
// [1] An Ultra Wideband Impulse Radio PHY Layer Model for Network Simulation,
// J. Rousselot, J.-D. Decotignie, Simulation: Transactions of the Society
// for Computer Simulation, 2010 (to appear).
// [2] A High-Precision Ultra Wideband Impulse Radio Physical Layer Model
// for Network Simulation, Jérôme Rousselot, Jean-Dominique Decotignie,
// Second International Omnet++ Workshop,Simu'TOOLS, Rome, 6 Mar 09.
// http://portal.acm.org/citation.cfm?id=1537714
//

#ifndef UWBIR_PHY_LAYER_H
#define UWBIR_PHY_LAYER_H

#include "PhyLayerBattery.h"
#include "RadioUWBIR.h"
#include "UWBIRStochasticPathlossModel.h"
#include "UWBIRIEEE802154APathlossModel.h"
#include "HostState.h"
#include "MacToPhyControlInfo.h"
#include "BaseUtility.h"


class DeciderUWBIRED;
class DeciderUWBIREDSyncOnAddress;
class DeciderUWBIREDSync;

#include "DeciderUWBIRED.h"
#include "DeciderUWBIREDSyncOnAddress.h"
#include "DeciderUWBIREDSync.h"

/**
 * @brief Physical layer that models an Ultra Wideband Impulse Radio wireless communication system.
 *
 * This class loads channel models and deliver frames to an UWB Decider. It is independent of the modulation technique,
 * as long as the frames are represented using the same approach as in IEEE802154A.h (Maximum Pulse Amplitude Estimation).
 *
 * Several channel models are possible: Ghassemzadeh-LOS, Ghassemadeh-NLOS (see UWBIRStochasticPathlossModel.h)
 * and the IEEE 802.15.4A UWB channel models that use the default power delay profile (see UWBIRIEEE802154APathlossModel.h).
 *
 * Currently, an energy detection receiver is modeled in UWBIRED.h.
 * Several synchronization logics have been implemented in derived classes:
 * see DeciderUWBIREDSync.h and and DeciderUWBIREDSyncOnAddress.h.
 *
 * If you want to add a novel receiver (e.g. coherent demodulation), either derive UWBIRED or write your own,
 * then add functionality in this module to load your decider.
 * The same apply for new channel models.
 *
 * To change the modulation, refer to UWBIRMac.h, IEEE802154A.h and UWBIRED.h.
 * To implement optional modes of IEEE802154A, refer to IEEE802154A.h.
 *
 * Refer to the following publications for more information:
 * [1] An Ultra Wideband Impulse Radio PHY Layer Model for Network Simulation,
 * J. Rousselot, J.-D. Decotignie, Simulation: Transactions of the Society
 * for Computer Simulation, 2010 (to appear).
 * [2] A High-Precision Ultra Wideband Impulse Radio Physical Layer Model
 * for Network Simulation, Jérôme Rousselot, Jean-Dominique Decotignie,
 * Second International Omnet++ Workshop,Simu'TOOLS, Rome, 6 Mar 09.
 * http://portal.acm.org/citation.cfm?id=1537714
 *
 * @ingroup ieee802154a
 * @ingroup phyLayer
 * @ingroup power
 */
class PhyLayerUWBIR : public BasePhyLayer
{
	friend class DeciderUWBIRED;

public:
	void initialize(int stage);
        PhyLayerUWBIR() : uwbpathloss(0), ieee802154AChannel(0) {}

    void finish();

    // this function allows to include common xml documents for ned parameters as ned functions
    static cDynamicExpression::Value ghassemzadehNLOSFunc(cComponent *context, cDynamicExpression::Value argv[], int argc) {
      const char * ghassemzadehnlosxml =
    		  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    		  "<root>"
    		  "<AnalogueModels>"
    		  "<AnalogueModel type=\"UWBIRStochasticPathlossModel\"><parameter name=\"PL0\" type=\"double\" value=\"-51\"/>"
    		        "<parameter name=\"mu_gamma\" type=\"double\" value=\"3.5\"/>"
                    "<parameter name=\"sigma_gamma\" type=\"double\" value=\"0.97\"/>"
                    "<parameter name=\"mu_sigma\" type=\"double\" value=\"2.7\"/>"
                    "<parameter name=\"sigma_sigma\" type=\"double\" value=\"0.98\"/>"
                    "<parameter name=\"isEnabled\" type=\"bool\" value=\"true\"/>"
                    "<parameter name=\"shadowing\" type=\"bool\" value=\"true\"/>"
                "</AnalogueModel>"
    		  "</AnalogueModels>"
    		  "</root>";
      cXMLParImpl xmlParser;
      xmlParser.parse(ghassemzadehnlosxml);  // from char* to xml
      cDynamicExpression::Value parameters(xmlParser.xmlValue(NULL)); // from xml to Value
      return parameters;
    }
    typedef cDynamicExpression::Value (*fptr) (cComponent *context, cDynamicExpression::Value argv[], int argc);
    static fptr ghassemzadehNLOSFPtr;
    //static cDynamicExpression::Value (*ghassemzadehNLOSFPtr) (cComponent *context, cDynamicExpression::Value argv[], int argc);



protected:

    UWBIRStochasticPathlossModel* uwbpathloss;
    UWBIRIEEE802154APathlossModel* ieee802154AChannel;
    DeciderUWBIRED* uwbdecider;

    virtual AirFrame *encapsMsg(cPacket *msg);

    virtual AnalogueModel* getAnalogueModelFromName(std::string name, ParameterMap& params);

    AnalogueModel* createUWBIRStochasticPathlossModel(ParameterMap & params);
    AnalogueModel* createUWBIRIEEE802154APathlossModel(ParameterMap & params);
    AnalogueModel* createIntensityModel(ParameterMap & params);
    virtual Decider* getDeciderFromName(std::string name, ParameterMap& params);
    virtual Radio* initializeRadio();

    RadioUWBIR* uwbradio;
    /**
     * called by Blackboard to inform of changes
     */
    virtual void receiveBBItem(int category, const BBItem *details, int scopeModuleId);

    virtual void handleAirFrame(cMessage* msg);

    virtual void switchRadioToRX() {
    	Enter_Method_Silent();
    	uwbradio->startReceivingFrame(simTime());
    	setRadioCurrent(uwbradio->getCurrentState());
    }

    virtual void switchRadioToSync() {
    	Enter_Method_Silent();
    	uwbradio->finishReceivingFrame(simTime());
    	setRadioCurrent(radio->getCurrentState());
    }

    virtual simtime_t setRadioState(int rs);

	/** @brief Number of power consuming activities (accounts).*/
	int numActivities;

	/** @brief The different currents in mA.*/
	double sleepCurrent, rxCurrent, decodingCurrentDelta, txCurrent, syncCurrent;

	/** @brief The differnet switching state currents in mA.*/
	double setupRxCurrent, setupTxCurrent, rxTxCurrent, txRxCurrent;

	/**
	 * @brief Defines the power consuming activities (accounts) of
	 * the NIC. Should be the same as defined in the decider.
	 */
	enum Activities {
		SLEEP_ACCT=0,
		RX_ACCT,  		//1
		TX_ACCT,  		//2
		SWITCHING_ACCT, //3
		SYNC_ACCT,		//4
	};

	virtual void setRadioCurrent(int rs);

	virtual void setSwitchingCurrent(int from, int to);

	/**
	 * @brief Captures changes in host state.
	 *
	 * Note: Does not yet cancel any ongoing transmissions if the
	 * state changes to off.
	 */
	virtual void handleHostState(const HostState& state);

	/**
	 * @brief Captures radio switches to adjust power consumption.
	 */
	virtual void finishRadioSwitching();

};

#endif
