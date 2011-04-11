#include "NodeTransLayer.h"

Define_Module(NodeTransLayer);

void NodeTransLayer::initialize(int stage) {
	BaseLayer::initialize(stage);
	if (stage == 0) {
	}
}
void NodeTransLayer::finish() {
}

/**
 * Redefine this function if you want to process messages from lower
 * layers before they are forwarded to upper layers
 **/
void NodeTransLayer::handleLowerMsg(cMessage* msg)
{
    sendUp(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from lower layers.
 **/
void NodeTransLayer::handleLowerControl(cMessage* msg)
{
	sendControlUp(msg);
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 **/
void NodeTransLayer::handleUpperMsg(cMessage* msg)
{
    sendDown(msg);
}

/**
 * Redefine this function if you want to process control messages
 * from upper layers.
 **/
void NodeTransLayer::handleUpperControl(cMessage* msg)
{
	sendControlDown(msg);
}
