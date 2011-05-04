package org.mixim.examples.ieee802154Narrow;

simple AnchorAppLayer
{
    parameters:
        double packetTime @unit(s);								// We'll have to change the packet time according to the Packet Length
        int phaseRepetitionNumber = default(-1); 				// Total number of full phases, -1 = unlimited
        int syncPacketsPerSyncPhase = default(4); 				// Determines how many times do we repeat all the slots per sync phase
    	int packetLength @unit("bit"); 							// Standard Packet Length, in some cases when we transmit info we'll have to change it
    	int destination = default(-1);							// Broadcast destination, usually -1
    	double syncPacketTime @unit(s) = default(1.5ms); 		// Max. duration of a Sync Packet, determines the slot size
    	double phase2VIPPercentage = default(0.4);				// Percentage of the time Phase Report + Phase VIP that the Phase VIP takes

    	bool syncInSlot = default(true);						// Indicates if the sync packets have to be slotted or not
    	double syncFirstMaxRandomTime @unit(s) = default(6ms); 	// First maximum time an anchor must wait to transmit the first sync packet in no slotted mode
    	double syncRestMaxRandomTimes @unit(s) = default(1ms); 	// Rest of maximum times an anchor must wait to transmit the rest of the sync packets in no slotted mode
    gates:
    	input lowerGateIn; 										// from NIC
        output lowerGateOut; 									// to NIC
        input lowerControlIn; 									// control from NIC
        output lowerControlOut; 								// control to NIC
}
