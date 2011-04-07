#include "AnchorTransLayer.h"

Define_Module(AnchorTransLayer);

void AnchorTransLayer::initialize(int stage) {
	BaseLayer::initialize(stage);
	if (stage == 0) {
	}
}
void AnchorTransLayer::finish() {
}

/**
 * Redefine this function if you want to process messages from lower
 * layers before they are forwarded to upper layers
 **/
void AnchorTransLayer::handleLowerMsg(cMessage* msg)
{
    sendUp(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from lower layers.
 **/
void AnchorTransLayer::handleLowerControl(cMessage* msg)
{
	sendControlUp(msg);
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 **/
void AnchorTransLayer::handleUpperMsg(cMessage* msg)
{
    sendDown(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from upper layers.
 **/
void AnchorTransLayer::handleUpperControl(cMessage* msg)
{
	sendControlDown(msg);
}
