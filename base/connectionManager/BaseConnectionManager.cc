#include "BaseConnectionManager.h"

#include "NicEntryDebug.h"
#include "NicEntryDirect.h"

#include "BaseWorldUtility.h"

#include "FindModule.h"

#include <cassert>

#ifndef ccEV
#define ccEV (ev.isDisabled()||!coreDebug) ? ev : ev << getName() << ": "
#endif

void BaseConnectionManager::initialize(int stage)
{
	//BaseModule::initialize(stage);

	if (stage == 0)
	{
		if(hasPar("coreDebug"))
			coreDebug = par("coreDebug").boolValue();
		else
			coreDebug = false;

		ccEV <<"initializing BaseConnectionManager\n";

		BaseWorldUtility* world = FindModule<BaseWorldUtility*>
										::findGlobalModule();

		assert(world != 0);

		playgroundSize = world->getPgs();
		useTorus = world->useTorus();

		if(hasPar("sendDirect"))
			sendDirect = par("sendDirect").boolValue();
		else
			sendDirect = false;

		maxInterferenceDistance = calcInterfDist();
		maxDistSquared = maxInterferenceDistance * maxInterferenceDistance;

		//----initialize node grid-----
		//step 1 - calculate dimension of grid
		//one cell should have at least the size of maxInterferenceDistance
		//but also should divide the playground in equal parts
		Coord dim((*playgroundSize) / maxInterferenceDistance);
		gridDim = GridCoord(dim);

		//A grid smaller or equal to 3x3 would mean that every cell has every
		//other cell as direct neighbor (if our playground is a torus, even if
		//not the most of the cells are direct neighbors of each other. So we
		//reduce the grid size to 1x1.
		if((gridDim.x <= 3) && (gridDim.y <= 3) && (gridDim.z <= 3))
		{
			gridDim.x = 1;
			gridDim.y = 1;
			gridDim.z = 1;
		} else {
			gridDim.x = std::max(1, gridDim.x);
			gridDim.y = std::max(1, gridDim.y);
			gridDim.z = std::max(1, gridDim.z);
		}

		//step 2 - initialize the matrix which represents our grid
		NicEntries entries;
		RowVector row;
		NicMatrix matrix;

		for (int i = 0; i < gridDim.z; ++i) {
			row.push_back(entries);			//copy empty NicEntries to RowVector
		}
		for (int i = 0; i < gridDim.y; ++i) {//fill the ColVector with copies of
			matrix.push_back(row);			 //the RowVector.
		}
		for (int i = 0; i < gridDim.x; ++i) {	//fill the grid with copies of
			nicGrid.push_back(matrix);			//the matrix.
		}
		ccEV << " using " << gridDim.x << "x" <<
							 gridDim.y << "x" <<
							 gridDim.z << " grid" << endl;

		//step 3 -	calculate the factor which maps the coordinate of a node
		//			to the grid cell

		if (gridDim.x == 1 &&							//if we use a 1x1 grid
			gridDim.y == 1 &&							//every coordinate is
			gridDim.z == 1) {							//mapped to (0,0, 0)
			findDistance = Coord(std::max(playgroundSize->getX(),
										  maxInterferenceDistance),
								 std::max(playgroundSize->getY(),
										  maxInterferenceDistance),
								 std::max(playgroundSize->getZ(),
										  maxInterferenceDistance));
		} else {
			//otherwise the factor is our playground divided by the number of
			//cells
			findDistance = Coord(playgroundSize->getX() / gridDim.x,
								 playgroundSize->getY() / gridDim.y,
								 playgroundSize->getZ() / gridDim.z);
		}

		//since the upper playground borders (at pg-size) are part of the
		//playground we have to assure that they are mapped to a valid
		//(the last) grid cell we do this by increasing the find distance
		//by a small value.
		//This also assures that findDistance is never zero.
		findDistance += Coord(EPSILON, EPSILON, EPSILON);

		//findDistance (equals cell size) has to be greater or equal
		//maxInt-distance
		assert(findDistance.getX() >= maxInterferenceDistance);
		assert(findDistance.getY() >= maxInterferenceDistance);
		assert(world->use2D()
			   || findDistance.getZ() >= maxInterferenceDistance);

		//playGroundSize has to be part of the playGround
		assert(GridCoord(*playgroundSize, findDistance).x == gridDim.x - 1);
		assert(GridCoord(*playgroundSize, findDistance).y == gridDim.y - 1);
		assert(GridCoord(*playgroundSize, findDistance).z == gridDim.z - 1);
		ccEV << "findDistance is " << findDistance.info() << endl;
	}
	else if (stage == 1)
	{

	}
}

BaseConnectionManager::GridCoord BaseConnectionManager
	::getCellForCoordinate(const Coord& c)
{
    return GridCoord(c, findDistance);
}

void BaseConnectionManager::updateConnections(int nicID,
											  const Coord* oldPos,
											  const Coord* newPos)
{
	GridCoord oldCell = getCellForCoordinate(*oldPos);
    GridCoord newCell = getCellForCoordinate(*newPos);

	checkGrid(oldCell, newCell, nicID );
}

BaseConnectionManager::NicEntries& BaseConnectionManager
	::getCellEntries(BaseConnectionManager::GridCoord& cell)
{
    return nicGrid[cell.x][cell.y][cell.z];
}

void BaseConnectionManager::registerNicExt(int nicID)
{
	NicEntry* nicEntry = nics[nicID];

	GridCoord cell = getCellForCoordinate(nicEntry->pos);

	ccEV <<" registering (ext) nic at loc " << cell.info() << std::endl;

	// add to matrix
	NicEntries& cellEntries = getCellEntries(cell);
    cellEntries[nicID] = nicEntry;
}

void BaseConnectionManager::checkGrid(BaseConnectionManager::GridCoord& oldCell,
                                      BaseConnectionManager::GridCoord& newCell,
                                      int id)

{

    // structure to find union of grid squares
    CoordSet gridUnion(74);

    // find nic at old position
    NicEntries& oldCellEntries = getCellEntries(oldCell);
    NicEntries::iterator it = oldCellEntries.find(id);
    NicEntry *nic = it->second;


    // move nic to a new position in matrix
    if(oldCell != newCell) {
    	oldCellEntries.erase(it);
    	getCellEntries(newCell)[id] = nic;
    }

	if((gridDim.x == 1) && (gridDim.y == 1) && (gridDim.z == 1)) {
		gridUnion.add(oldCell);
    } else {
		//add grid around oldPos
		fillUnionWithNeighbors(gridUnion, oldCell);


        if(oldCell != newCell) {
            //add grid around newPos
            fillUnionWithNeighbors(gridUnion, newCell);
        }
    }

    GridCoord* c = gridUnion.next();
    while(c != 0) {
		ccEV << "Update cons in [" << c->info() << "]" << endl;
		updateNicConnections(getCellEntries(*c), nic);
		c = gridUnion.next();
    }
}

int BaseConnectionManager::wrapIfTorus(int value, int max) {
	if(value < 0) {
		if(useTorus) {
			return max + value;
		} else {
			return -1;
		}
	} else if(value >= max) {
		if(useTorus) {
			return value - max;
		} else {
			return -1;
		}
	} else {
		return value;
	}
}

void BaseConnectionManager::fillUnionWithNeighbors(CoordSet& gridUnion,
												   GridCoord cell)
{
	for(int iz = (int)cell.z - 1; iz <= (int)cell.z + 1; iz++) {
		if(iz != cell.z && cell.use2D) {
			continue;
		}
		int cz = wrapIfTorus(iz, gridDim.z);
		if(cz == -1) {
			continue;
		}
		for(int ix = (int)cell.x - 1; ix <= (int)cell.x + 1; ix++) {
			int cx = wrapIfTorus(ix, gridDim.x);
			if(cx == -1) {
				continue;
			}
			for(int iy = (int)cell.y - 1; iy <= (int)cell.y + 1; iy++) {
				int cy = wrapIfTorus(iy, gridDim.y);
				if(cy != -1) {
					if(cell.use2D) {
						gridUnion.add(GridCoord(cx, cy));
					} else {
						gridUnion.add(GridCoord(cx, cy, cz));
					}
				}
			}
		}
	}
}

void BaseConnectionManager::updateNicConnections(NicEntries& nmap,
												 NicEntry* nic)
{
    int id = nic->nicId;

    for(NicEntries::iterator i = nmap.begin(); i != nmap.end(); ++i)
    {
		NicEntry* nic_i = i->second;

        // no recursive connections
        if ( nic_i->nicId == id ) continue;

		double distance;

        if(useTorus)
        {
        	distance = nic->pos.sqrTorusDist(nic_i->pos, playgroundSize);
        } else {
        	distance = nic->pos.sqrdist(nic_i->pos);
        }

        bool inRange = (distance <= maxDistSquared);
        bool connected = nic->isConnected(nic_i);


        if ( inRange && !connected ){
            // nodes within communication range: connect
            // nodes within communication range && not yet connected
            ccEV << "nic #" << id << " and #" << nic_i->nicId
            	 << " are in range" << endl;
            nic->connectTo( nic_i );
            nic_i->connectTo( nic );
        }
        else if ( !inRange && connected ){
            // out of range: disconnect
            // out of range, and still connected
            ccEV << "nic #" << id << " and #" << nic_i->nicId
            	 << " are NOT in range" << endl;
            nic->disconnectFrom( nic_i );
            nic_i->disconnectFrom( nic );
        }
    }
}

bool BaseConnectionManager::registerNic(cModule* nic,
										ChannelAccess* chAccess,
										const Coord* nicPos)
{
	assert(nic != 0);

	int nicID = nic->getId();
	ccEV << " registering nic #" << nicID << endl;

	// create new NicEntry
	NicEntry *nicEntry;

	if(sendDirect)
		nicEntry = new NicEntryDirect(coreDebug);
	else
		nicEntry = new NicEntryDebug(coreDebug);

	// fill nicEntry
	nicEntry->nicPtr = nic;
	nicEntry->nicId = nicID;
	nicEntry->hostId = nic->getParentModule()->getId();
	nicEntry->pos = nicPos;
	nicEntry->chAccess = chAccess;

	// add to map
	nics[nicID] = nicEntry;

	registerNicExt(nicID);

	updateConnections(nicID, nicPos, nicPos);

	return sendDirect;
}

bool BaseConnectionManager::unregisterNic(cModule* nicModule)
{
	assert(nicModule != 0);

	// find nicEntry
	int nicID = nicModule->getId();
	ccEV << " unregistering nic #" << nicID << endl;

	//we assume that the module was previously registered with this CM
	//TODO: maybe change this to an omnet-error instead of an assertion
	assert(nics.find(nicID) != nics.end());
	NicEntry* nicEntry = nics[nicID];

	// get all affected grid squares
	CoordSet gridUnion(74);
	GridCoord cell = getCellForCoordinate(nicEntry->pos);
	if((gridDim.x == 1) && (gridDim.y == 1) && (gridDim.z == 1)) {
		gridUnion.add(cell);
	} else {
		fillUnionWithNeighbors(gridUnion, cell);
	}

	// disconnect from all NICs in these grid squares
	GridCoord* c = gridUnion.next();
	while(c != 0) {
		ccEV << "Update cons in [" << c->info() << "]" << endl;
		NicEntries& nmap = getCellEntries(*c);
		for(NicEntries::iterator i = nmap.begin(); i != nmap.end(); ++i) {
			NicEntry* other = i->second;
			if (other == nicEntry) continue;
			if (!other->isConnected(nicEntry)) continue;
			other->disconnectFrom(nicEntry);
		}
		c = gridUnion.next();
	}

	// erase from grid
	NicEntries& cellEntries = getCellEntries(cell);
	cellEntries.erase(nicID);

	// erase from list of known nics
	nics.erase(nicID);

	return true;
}

void BaseConnectionManager::updateNicPos(int nicID, const Coord* newPos)
{
	NicEntry* nicEntry = nics[nicID];
	if(nicEntry == 0)
		error("No nic with this ID is registered with this ConnectionManager.");

    Coord oldPos = nicEntry->pos;
    nicEntry->pos = newPos;

	updateConnections(nicID, &oldPos, newPos);
}

const NicEntry::GateList& BaseConnectionManager::getGateList(int nicID)
{
	return nics[nicID]->getGateList();
}

const cGate* BaseConnectionManager::getOutGateTo(const NicEntry* nic,
												 const NicEntry* targetNic)
{
    return nics[nic->nicId]->getOutGateTo(targetNic);
}

BaseConnectionManager::~BaseConnectionManager()
{
	for (NicEntries::iterator ne = nics.begin(); ne != nics.end(); ne++)
	{
		delete ne->second;
	}
}
