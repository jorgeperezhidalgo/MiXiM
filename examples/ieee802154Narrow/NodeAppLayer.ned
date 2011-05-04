package org.mixim.examples.ieee802154Narrow;

simple NodeAppLayer
{
    parameters:
        double packetTime @unit(s);								// We'll have to change the packet time according to the Packet Length
        int syncPacketsPerSyncPhase = default(4); 				// Determines how many times do we repeat all the slots per sync phase
    	int packetLength @unit("bit"); 							// Standard Packet Length, in some cases when we transmit info we'll have to change it
    	int destination = default(-1);							// Broadcast destination, usually -1
    	double syncPacketTime @unit(s) = default(1.5ms); 		// Max. duration of a Sync Packet, determines the slot size
    	double phase2VIPPercentage = default(0.4);				// Percentage of the time Phase Report + Phase VIP that the Phase VIP takes

    	int nodeConfig = default(1); 							// Which of the four types is the MN, check the MobileNodeType in .h to see the possible types
    	int offsetPhases = default(0);							// Number of Offset phases to start the first active phase (to avoid that all MN transmit at the same phase)
		int offsetSyncPhases = default(0);						// Number of sync phases we don't read at the beggining of the first active phase, it can be 0, 1 or 2, with 3 we would set an active phase less
    	int activePhases = default(1); 							// Configuration number of active phases, where we perform the standard functions of every type
    	int inactivePhases = default(0); 						// Configuration number of inactive phases, where we do nothing or extra reports if scheduled
		int offsetReportPhases = default(2); 					// Number of Offset full phases to start the first extra report full phase (to avoid that all MN transmit at the same full phase)
		int reportPhases = default(10); 						// Configuration number to know every how many full phases do we make an extra report
		bool makeExtraReport = default(true); 					// True we make it (listen sync and report), false we don't (just listen sync, for synchronization just)
		int NumberOfBroadcasts = default(3); 					// Number of broadcasts (VIP or normal) done by a MN type 3 or 4
		int positionsToSave = default(5); 						// Number of positions to save for the MN in Type 2
		bool centralized = default(true); 						// In Type 1, who calculates the position? True = Computer, False = AN Selected
		int askFrequency = default(1);							// Every askFrequency Extra Reports we activate the ASK flag to make a request 
    gates:
    	input lowerGateIn; 										// from NIC
        output lowerGateOut; 									// to NIC
        input lowerControlIn; 									// control from NIC
        output lowerControlOut; 								// control to NIC
}
