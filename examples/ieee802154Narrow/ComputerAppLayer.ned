package org.mixim.examples.ieee802154Narrow;

simple ComputerAppLayer
{
    parameters:
        double packetTime @unit(s);						// We'll have to change the packet time according to the Packet Length
        int syncPacketsPerSyncPhase = default(4); 		// Determines how many times do we repeat all the slots per sync phase
    	int packetLength @unit("bit");					// Standard Packet Length, in some cases when we transmit info we'll have to change it
    	int destination = default(-1);					// Broadcast destination, usually -1
    	double syncPacketTime @unit(s) = default(1.5ms);// Max. duration of a Sync Packet, determines the slot size
    	double phase2VIPPercentage = default(0.4);		// Percentage of the time Phase Report + Phase VIP that the Phase VIP takes
    gates:
    	input lowerGateIn; 								// from NIC
        output lowerGateOut; 							// to NIC
        input lowerControlIn; 							// control from NIC
        output lowerControlOut; 						// control to NIC
}
