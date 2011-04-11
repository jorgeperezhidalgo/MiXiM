#include "ComputerTransLayer.h"

Define_Module(ComputerTransLayer);

void ComputerTransLayer::initialize(int stage) {
	BaseLayer::initialize(stage);
	if (stage == 0) {
	}
}
void ComputerTransLayer::finish() {
}

/**
 * Redefine this function if you want to process messages from lower
 * layers before they are forwarded to upper layers
 **/
void ComputerTransLayer::handleLowerMsg(cMessage* msg)
{
    sendUp(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from lower layers.
 **/
void ComputerTransLayer::handleLowerControl(cMessage* msg)
{
	sendControlUp(msg);
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 **/
void ComputerTransLayer::handleUpperMsg(cMessage* msg)
{
    sendDown(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from upper layers.
 **/
void ComputerTransLayer::handleUpperControl(cMessage* msg)
{
	sendControlDown(msg);
}
