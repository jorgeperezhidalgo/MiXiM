//**************************************************************************
// * file:        SyncPkt.msg
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
// * description: - base class for application layer packets for sync
// *              - subclass to create your own application layer packets
// **************************************************************************/


// Sync application packet definition
//
// subclass if you want to create your own application layer packet
// class
//
// @author Jorge Perez Hidalgo
packet SyncPkt
{
	int sequenceId = 0;
	int srcAddr = 0;
    int8 status = 0;
	int16 posX = 0;
	int16 posY = 0;
	int16 posZ = 0;
	int32 timestamp = 0;
}
