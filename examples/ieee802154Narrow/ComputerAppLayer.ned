//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package org.mixim.examples.ieee802154Narrow;

//
// TODO auto-generated module
//
simple ComputerAppLayer
{
    parameters:
        double packetTime @unit(s);
        bool debug; // debug switch
        int phaseRepetitionNumber = default(-1); // How many full phases should we do
        int syncPacketsPerSyncPhase = default(4); // How many sync packets to send every sync phase
    	int packetLength @unit("bit"); // length of the generated packet (in bits)
    	int destination = default(-1);
    	double syncPacketTime @unit(s) = default(1.5ms); 
    	double phase2VIPPercentage = default(0.4);
    	bool syncInSlot = default(true); // Indicates if the sync packets have to be slotted or not
    gates:
    	input lowerGateIn; // from NIC
        output lowerGateOut; // to NIC
        input lowerControlIn; // control from NIC
        output lowerControlOut; // control to NIC
}
