//**************************************************************************
// * file:        ApplPkt.msg
// *
// * author:      Jorge Perez Hidalgo
// *
// * copyright:   (C) 2011 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Dresden, Germany.
// *
// *              This program is free software; you can redistribute it 
// *              and/or modify it under the terms of the GNU General Public 
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later 
// *              version.
// *              For further information see file COPYING 
// *              in the top level directory
// **************************************************************************
// * part of:     framework implementation developed by tkn
// * description: - base class for application layer packets
// *              - subclass to create your own application layer packets
// **************************************************************************/


// Basic application packet definition
//
// subclass if you want to create your own application layer packet
// class
//
// @author Jorge Perez Hidalgo
packet ApplPkt
{
    int destAddr = -1; 				// destination address
    int srcAddr = -1; 				// source address
    int sequenceId = 0;				// a counter to identify the packet
    int priority = 0; 				// Priority of the packet
    int8 status = 0;
	int16 posX = 0;
	int16 posY = 0;
	int16 posZ = 0;
	int32 timestamp = 0;
	// This two address are there because: when a MN transmits the destAddr is the selected AN, but the real destination can be the computer
	// And also when a computer sends something to a MN, the destAddr is the selected AN but the real one is the MN, and more cases like this

	int realDestAddr = -1;			// Real destination, in case the packet has to be routed from MN to Computer through an AN
	int realSrcAddr = -1;			// Real source, in case the packet has to be routed from Computer to MN through an AN
	int retransmisionCounterBO = 0;	// This variable counts the number or retransmission of this packet due to backoff
	int retransmisionCounterACK = 0;// This variable counts the number or retransmission of this packet due to lack of ACK
	bool CSMA = true;				// Variable to activate or deactivate the CSMA
	bool askForRequest = false;		// This FLAG in the packet tells the AN that in the next full phase (period) the Mobile Node will ask for some information and the AN has to have it ready
	bool requestPacket = false;		// This FLAG in the packet tells the AN that it has to answer the Mobile Node with some info and if it doesn't have anything ask anyway saying the info is not yet there
	int numberOfBroadcasts = 0;		// When a mobile node sends a Broadcast, here it will write how many total broadcasts would it send
}
