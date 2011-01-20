#include "TestMacLayer.h"
#include <DeciderToPhyInterface.h>
Define_Module(TestMacLayer);

//---omnetpp part----------------------

//---intialisation---------------------
void TestMacLayer::initialize(int stage) {
	if(stage == 0) {
		if (simulation.getSystemModule()->par("showPassed"))
			displayPassed = true;
		else
			displayPassed = false;

		myIndex = findHost()->getIndex();

		dataOut = findGate("lowerGateOut");
		dataIn = findGate("lowerGateIn");

		controlOut = findGate("lowerControlOut");
		controlIn = findGate("lowerControlIn");

		run = simulation.getSystemModule()->par("run");

		init("mac" + toString(myIndex));

		testPhy = FindModule<TestPhyLayer*>::findSubModule(this->getParentModule());
		phy = testPhy;

		planTests();

	} else if(stage == 1) {
		runTests(TEST_START);
	}
}

void TestMacLayer::onAssertedMessage(int state, const cMessage* msg) {
	Enter_Method_Silent();

	runTests(state, msg);
}

void TestMacLayer::planTests()
{
	if(run == 6 && myIndex == 0)
	{
		/*
		Test if decider has access to already ended AirFrames during a
		ChannelSenseRequest which started during the AirFrame but ended after
		the AirFrame. This tests the use of ChannelInfo's record-feature during
		ChannelSenseRequests.
		Testoutline:
		*/
		planTest("1.", "Host1 sends AirFrame A to Host2");
		planTest("2.", "Host2 starts receiving AirFrame A");
		planTest("3.", "Host2 starts a ChannelSense");
		planTest("3.1", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
					    "ChannelSense duration which should return AirFrame A");
		planTest("3.2", "Host2 starts a ChannelSense");
		planTest("4.", "Host2 completes reception of AirFrame A");
		planTest("5.", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
					   "ChannelSense duration which should return AirFrame A");
		planTest("6.", "Host2 starts a ChannelSense");
		planTest("7.", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
					  "ChannelSense duration which should return none");
	}
}

//---test handling and redirection------------

/**
 * Redirects test handling to the "testRunx()"-methods
 * dependend on the current run.
 */
void TestMacLayer::runTests(int state, const cMessage* msg) {
	if(run == 1)
		testRun1(state, msg);
	else if(run == 2)
		testRun2(state, msg);
	else if(run == 3)
		testRun3(state, msg);
	else if (run == 5)
		testRun5(state, msg);
	else if (run == 6)
		testRun6(state, msg);
}

/**
 * Testhandling for run 1:
 * - check getChannelState()
 * - check switchRadio() method of MacToPhyInterface
 * - check sending on not TX mode
 * - check channel sense request
 */
void TestMacLayer::testRun1(int stage, const cMessage* msg){
	switch(stage) {
	case TEST_START:
		testGetChannelState();
	case RUN1_TEST_ON_RX:
		testSwitchRadio(stage);
		testChannelSense();
		break;
	case RUN1_TEST_ON_SLEEP:
		testSendingOnNotTX();
		break;
	default:
		fail("Invalid stage for run 1:" + toString(stage));
		break;
	}
}

/**
 * Testhandling for run 2:
 * - check sending on already sending
 */
void TestMacLayer::testRun2(int stage, const cMessage* msg){
	switch(stage) {
	case TEST_START:
		waitForTX(RUN2_SWITCH_TO_TX);
		break;
	case RUN2_SWITCH_TO_TX:{
		int state = phy->getRadioState();
		assertEqual("Radio is in TX.", Radio::TX, state);

		MacPkt* pkt = createMacPkt(1.0);
		sendDown(pkt);

		assertMessage("MacPkt at Phy layer.", TEST_MACPKT, simTime(), "phy0");
		//we don't assert any txOver message because we asume that the run
		//has been canceled before
		continueIn(0.5, RUN2_DURING_SENDING);
		break;
	}
	case RUN2_DURING_SENDING: {
		MacPkt* pkt = createMacPkt(1.0);
		sendDown(pkt);

		assertMessage("MacPkt at Phy layer.", TEST_MACPKT, simTime(), "phy0");
		//the run should be canceled after the asserted message, this is checked
		//indirectly by not asserting the txOverMessage
		break;
	}
	default:
		fail("Invalid stage for run 2:" + toString(stage));
		break;
	}
}

/**
 * Testhandling for run 3:
 * - check valid sending of a packet to 3 recipients
 * - check getChannelInfo
 */
void TestMacLayer::testRun3(int stage, const cMessage* msg){
	switch(myIndex) {
	case 0:
		testSending1(stage, msg);
		break;
	case 1:
	case 2:
	case 3:
		testChannelInfo(stage);
		break;
	default:
		fail("No handling for test run 3 for this host index: " + toString(myIndex));
		break;
	}
}

/**
 * Testhandling for run 5:
 *
 * - check getChannelState()
 * - check channel sensing
 *
 * TODO: for now the methods called here are empty
 *
 * Testing SNRThresholdDecider is done in initialize of the TestPhyLayer
 * without using the simulation so far, i.e. among other things TestPhyLayer
 * overriding the methods of DeciderToPhyInterface implemented by
 * BasePhyLayer. This way we check whether SNRThresholdDecider makes correct calls
 * on the Interface and return specific testing values to SNRThresholdDecider.
 *
 * Sending real AirFrames over the channel that SNRThresholdDecider can obtain
 * by calling the DeciderToPhyInterface will be done later.
 */
void TestMacLayer::testRun5(int stage, const cMessage* msg)
{
	switch (stage) {
		case TEST_START:
			testGetChannelStateWithBD();
			// NOTE: there is no break here!
		case RUN5_TEST_ON_RX:
			// testSwitchRadio(stage);
			testChannelSenseWithBD();
			break;
		default:
			break;
	}
}

void TestMacLayer::testRun6(int stage, const cMessage* msg)
{

	if(stage == TEST_START && myIndex == 0) {
// planTest("1.", "Host1 sends AirFrame A to Host2");
		waitForTX(RUN6_TEST_ON_TX);
	} else if(stage == RUN6_TEST_ON_TX && myIndex == 0) {
		MacPkt* pkt = createMacPkt(1.0);
		sendDown(pkt);
		testForMessage("1.", TEST_MACPKT, simTime(), "phy0");

// planTest("2.", "Host2 starts receiving AirFrame A");
		testForMessage("2.", BasePhyLayer::AIR_FRAME, simTime(), "phy1");

//planTest("3.", "Host2 starts a ChannelSense");
	} else if(stage == TEST_START && myIndex == 1) {
		waitForMessage(RUN6_TEST_ON_DECIDER1,
					   "First process of AirFrame at Decider",
					   BasePhyLayer::AIR_FRAME,
					   3.5, "decider1");
	} else if(stage == RUN6_TEST_ON_DECIDER1 && myIndex == 1) {
		ChannelSenseRequest* req = new ChannelSenseRequest();
		req->setKind(MacToPhyInterface::CHANNEL_SENSE_REQUEST);
		req->setSenseTimeout(0.5);
		req->setSenseMode(UNTIL_TIMEOUT);
		send(req, controlOut);
		testForMessage(	"3.", MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "phy1");
		assertMessage(	"ChannelSense at decider.",
						MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "decider1");

//planTest("3.1", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
//				"ChannelSense duration which should return AirFrame A");
		waitForMessage(RUN6_TEST_ON_CSR_RESULT_1, "Channel sense answer at mac",
					   MacToPhyInterface::CHANNEL_SENSE_REQUEST,
					   simTime() + 0.5);
	} else if(stage == RUN6_TEST_ON_CSR_RESULT_1 && myIndex == 1) {
		const ChannelSenseRequest* answer
				= dynamic_cast<const ChannelSenseRequest*>(msg);
		assertTrue("Received ChannelSenseRequest answer.", answer != NULL);
		manager->testForFalse("3.1", answer->getResult().isIdle());

//planTest("3.2", "Host2 starts a ChannelSense");
		ChannelSenseRequest* req = new ChannelSenseRequest();
		req->setKind(MacToPhyInterface::CHANNEL_SENSE_REQUEST);
		req->setSenseTimeout(1.0);
		req->setSenseMode(UNTIL_TIMEOUT);
		send(req, controlOut);
		testForMessage(	"3.2", MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "phy1");
		assertMessage(	"ChannelSense at decider.",
						MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "decider1");

// planTest("4.", "Host2 completes reception of AirFrame A");
		assertMessage("Transmission over message at phy",
					  BasePhyLayer::TX_OVER,
					  simTime() + 0.5, "phy0");
		assertMessage("Transmission over message from phy",
					  BasePhyLayer::TX_OVER,
					  simTime() + 0.5, "mac0");
		testForMessage("4.", BasePhyLayer::AIR_FRAME, simTime() + 0.5, "phy1");

// planTest("5.", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames"
//		   		  "during ChannelSense duration which should return AirFrame A");
		//decider will return busy if AirFrame was returned otherwise idle
		waitForMessage(RUN6_TEST_ON_CSR_RESULT_2, "Channel sense answer at mac",
					   MacToPhyInterface::CHANNEL_SENSE_REQUEST,
					   simTime() + 1.0);
	} else if(stage == RUN6_TEST_ON_CSR_RESULT_2 && myIndex == 1) {
		const ChannelSenseRequest* answer
				= dynamic_cast<const ChannelSenseRequest*>(msg);
		assertTrue("Received ChannelSenseRequest answer.", answer != NULL);
		manager->testForFalse("5.", answer->getResult().isIdle());

// planTest("6.", "Host2 starts a ChannelSense");
		ChannelSenseRequest* req = new ChannelSenseRequest();
		req->setKind(MacToPhyInterface::CHANNEL_SENSE_REQUEST);
		req->setSenseTimeout(0.5);
		req->setSenseMode(UNTIL_TIMEOUT);
		send(req, controlOut);
		testForMessage(	"6.", MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "phy1");
		assertMessage(	"ChannelSense at decider.",
						MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "decider1");

// planTest("7.", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
//  			 "ChannelSense duration which should return none");
		waitForMessage(RUN6_TEST_ON_CSR_RESULT_3, "Channel sense answer at mac",
					   MacToPhyInterface::CHANNEL_SENSE_REQUEST,
					   simTime() + 0.5);

	} else if(stage == RUN6_TEST_ON_CSR_RESULT_3 && myIndex == 1) {
		const ChannelSenseRequest* answer
				= dynamic_cast<const ChannelSenseRequest*>(msg);
		assertTrue("Received ChannelSenseRequest answer.", answer != NULL);
		manager->testForTrue("7.", answer->getResult().isIdle());
	}


	/*
	planTest("1.", "Host1 sends AirFrame A to Host2");
	planTest("2.", "Host2 starts receiving AirFrame A");
	planTest("3.", "Host2 starts a ChannelSense");
	planTest("4.", "Host2 completes reception of AirFrame A");
	planTest("5.", "Host2 ends ChannelSense and asks ChannelInfo for AirFrames during "
				   "ChannelSense duration which should return AirFrame A");
	*/
}

//---run 1 test-----------------------------

/**
 * Test the method "getChannelState()".
 */
void TestMacLayer::testGetChannelState() {
	ChannelState state = phy->getChannelState();
	assertTrue("First channelstates idle state should be true", state.isIdle());
	assertClose("First channelstates rssi.", 1.0, state.getRSSI());

	state = phy->getChannelState();
	assertFalse("Second channelstate should be false", state.isIdle());
	assertClose("Second channelstates rssi.", 2.0, state.getRSSI());

	state = phy->getChannelState();
	assertTrue("Third channelstate should be true", state.isIdle());
	assertClose("Third channelstates rssi.", 3.0, state.getRSSI());

	state = phy->getChannelState();
	assertFalse("Fourth channelstate should be false", state.isIdle());
	assertClose("Fourth channelstates rssi.", 4.0, state.getRSSI());
}
/**
 * Tests for switchRadio interface methods
 */
void TestMacLayer::testSwitchRadio(int stage) {
	int expState;
	simtime_t expTime;
	int nextState;
	int nextStage;

	switch(stage) {
	case TEST_START:
		expState = Radio::SLEEP;
		nextState = Radio::RX;
		expTime = 3.0;
		nextStage = RUN1_TEST_ON_RX;
		break;
	case RUN1_TEST_ON_RX:
		expState = Radio::RX;
		nextState = Radio::SLEEP;
		expTime = 1.5;
		nextStage = RUN1_TEST_ON_SLEEP;
		break;

	default:
		break;
	}

	int state = phy->getRadioState();
	assertEqual("Radio starts in SLEEP.", expState, state);
	simtime_t switchTime = phy->setRadioState(nextState);
	assertEqual("Correct switch time to RX.", expTime, switchTime);


	assertMessage(	"SWITCH_OVER message at phy.",
					BasePhyLayer::RADIO_SWITCHING_OVER,
					simTime() + switchTime,
					"phy" + toString(myIndex));
	waitForMessage(	nextStage,
					"SWITCH_OVER message.",
					BasePhyLayer::RADIO_SWITCHING_OVER,
					simTime() + switchTime);

	switchTime = phy->setRadioState(Radio::RX);
	assertTrue("Invalid switchtime because already switching.", switchTime < 0.0);
}

void TestMacLayer::testChannelSense() {
	ChannelSenseRequest* req = new ChannelSenseRequest();
	req->setKind(MacToPhyInterface::CHANNEL_SENSE_REQUEST);
	req->setSenseTimeout(0.5f);
	req->setSenseMode(UNTIL_TIMEOUT);
	send(req, controlOut);
	assertMessage(	"ChannelSense at phy layer.",
					MacToPhyInterface::CHANNEL_SENSE_REQUEST,
					simTime(), "phy" + toString(myIndex));
	assertMessage(	"ChannelSense at decider.",
						MacToPhyInterface::CHANNEL_SENSE_REQUEST,
						simTime(), "decider" + toString(myIndex));
}

void TestMacLayer::testSendingOnNotTX() {
	int state = phy->getRadioState();
	assertNotEqual("Radio is not in TX.", Radio::TX, state);

	MacPkt* pkt = createMacPkt(1.0);
	sendDown(pkt);

	assertMessage("MacPkt at Phy layer.", TEST_MACPKT, simTime(), "phy0");
}

//---run 2 tests----------------------------


//---run 3 tests----------------------------

void TestMacLayer::testChannelInfo(int stage) {
	switch(stage) {
	case TEST_START: {
		DeciderToPhyInterface::AirFrameVector v;
		testPhy->getChannelInfo(0.0, simTime(), v);

		assertTrue("No AirFrames on channel.", v.empty());
		break;
	}
	case RUN3_TEST_ON_DECIDER1:
	case RUN3_TEST_ON_DECIDER2:
	case RUN3_TEST_ON_DECIDER3:{
		if(myIndex ==( stage - RUN3_TEST_ON_DECIDER1 + 1)) {
			DeciderToPhyInterface::AirFrameVector v;
			testPhy->getChannelInfo(0.0, simTime(), v);

			assertFalse("AirFrames on channel.", v.empty());
		}
		break;
	}
	default:
		fail("TestChannelInfo: Unknown stage.");
		break;
	}
	//displayPassed = false;
}

void TestMacLayer::testSending1(int stage, const cMessage* lastMsg) {
	switch(stage) {
	case TEST_START: {
		waitForTX(RUN3_TEST_ON_TX);

		break;
	}
	case RUN3_TEST_ON_TX:{
		MacPkt* pkt = createMacPkt(1.0);

		sendDown(pkt);

		assertMessage("MacPkt at Phy layer.", TEST_MACPKT, simTime(), "phy0");

		assertMessage("First receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime(), "phy1");
		assertMessage("First receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime(), "phy2");
		assertMessage("First receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime(), "phy3");
		assertMessage("End receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime() + 1.0, "phy1");
		assertMessage("End receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime() + 1.0, "phy2");
		assertMessage("End receive of AirFrame", BasePhyLayer::AIR_FRAME, simTime() + 1.0, "phy3");

		waitForMessage(RUN3_TEST_ON_DECIDER1, "First process of AirFrame at Decider", BasePhyLayer::AIR_FRAME, simTime(), "decider1");
		waitForMessage(RUN3_TEST_ON_DECIDER2, "First process of AirFrame at Decider", BasePhyLayer::AIR_FRAME, simTime(), "decider2");
		waitForMessage(RUN3_TEST_ON_DECIDER3, "First process of AirFrame at Decider", BasePhyLayer::AIR_FRAME, simTime(), "decider3");

		assertMessage("Transmission over message at phy", BasePhyLayer::TX_OVER, simTime() + 1.0, "phy0");
		assertMessage("Transmission over message from phy", BasePhyLayer::TX_OVER, simTime() + 1.0);
		break;
	}
	case RUN3_TEST_ON_DECIDER1:
	case RUN3_TEST_ON_DECIDER2:
	case RUN3_TEST_ON_DECIDER3:{
		TestMacLayer* mac = dynamic_cast<TestMacLayer*>(manager->getModule("mac" + toString((stage - RUN3_TEST_ON_DECIDER1)+ 1)));
		mac->onAssertedMessage(stage, 0);
		break;
	}
	default:
		fail("Unknown stage");
		break;
	}
}

//---run 5 tests----------------------------

void TestMacLayer::testChannelSenseWithBD()
{
	// empty for now

}

void TestMacLayer::testGetChannelStateWithBD()
{
	// empty for now
}

//---utilities------------------------------

void TestMacLayer::continueIn(simtime_t time, int nextStage){
	scheduleAt(simTime() + time, new cMessage(0, 23242));
	waitForMessage(	nextStage,
					"Waiting for " + toString(time) + "s.",
					23242,
					simTime() + time);
}

void TestMacLayer::waitForTX(int nextStage) {
	simtime_t switchTime = phy->setRadioState(Radio::TX);
	assertTrue("A valid switch time.", switchTime >= 0.0);


	assertMessage(	"SWITCH_OVER to TX message at phy.",
					BasePhyLayer::RADIO_SWITCHING_OVER,
					simTime() + switchTime,
					"phy" + toString(myIndex));
	waitForMessage(	nextStage,
					"SWITCH_OVER to TX message.",
					BasePhyLayer::RADIO_SWITCHING_OVER,
					simTime() + switchTime);
}

void TestMacLayer::sendDown(MacPkt* pkt) {
	send(pkt, dataOut);
}

MacPkt* TestMacLayer::createMacPkt(simtime_t length) {
	Signal* s = new Signal(simTime(), length);
	MacToPhyControlInfo* ctrl = new MacToPhyControlInfo(s);
	MacPkt* res = new MacPkt();
	res->setControlInfo(ctrl);
	res->setKind(TEST_MACPKT);
	return res;
}
