package org.mixim.examples.ieee802154Narrow;

simple ComputerAppLayer
{
    parameters:
        int syncPacketsPerSyncPhase = default(4); 		// Determines how many times do we repeat all the slots per sync phase
    	int destination = default(-1);					// Broadcast destination, usually -1
    	double syncPacketTime @unit(s) = default(1.5ms);// Max. duration of a Sync Packet, determines the slot size
    	double phase2VIPPercentage = default(0.4);		// Percentage of the time Phase Report + Phase VIP that the Phase VIP takes

		// All of them except syncPacket have a + 6 bytes due to the extra bytes needed by the long address for Mobile Nodes, it corresponds to MAC layer but here is easier to add the offset    	
    	int syncPacketLength @unit("bit") = default(88bit);			// Sync Packet Length, includes 1 byte status, 4 byte timestamp and 2 + 2 + 2 bytes possition
    	int normalReportPacketLength @unit("bit") = default(56bit); // Rutinary Report Packet Length, includes 1 byte status and should be added 2 bytes x each listened Anchor, only exists in MN type 1 and 4
    	int type2ReportPacketLength @unit("bit") = default(56bit);	// MN type 2 Report Packet Length, includes 1 byte status and should be added (2 + 2 + 2 = 6) position bytes x each position calculated, only MN type 2
    	int askReportPacketLength @unit("bit") = default(80bit);	// When a report or extra report has the flag ASK activated, 10 bytes must be added to take into account the info sent about which we ask
    	int requestPacketLength @unit("bit") = default(8bit);		// When a report or extra report has the flag Request activated, 1 byte must be added to take into account the info sent about which we ask
    	int broadcastPacketLength @unit("bit") = default(64bit);	// Broadcast Packet Length, includes 1 byte status and 1 aditional byte information. This is the broadcast packet sent by MN type 3 and 4
    	int answerANtoMNPacketLength @unit("bit") = default(136bit);// Report sent by AN to MN when MN makes a request, includes 1 byte status and 10 bytes info.
    	int bcastMixANPacketLength @unit("bit") = default(72bit); 	// Report sent by AN when it receives broadcasts from a MN, sends a report per broadcast, includes 1 byte status and 2 bytes RSSI info.
    gates:
    	input lowerGateIn; 								// from NIC
        output lowerGateOut; 							// to NIC
        input lowerControlIn; 							// control from NIC
        output lowerControlOut; 						// control to NIC
}
