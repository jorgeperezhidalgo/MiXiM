package org.mixim.base.modules;

// Module which is responsible for mobility
// related information like position and movement
// BaseMobility itself defines a static mobility
// pattern (means only a position, no movement).
// Mobility modules which extend from this module
// will define more complex movement patterns.
simple BaseMobility like IBaseMobility
{
    parameters:
        bool notAffectedByHostState = true;
        bool coreDebug; // debug switch for the core framework
        double x; // x coordinate of the nodes' position (-1 = random)
        double y; // y coordinate of the nodes' position (-1 = random)
        double z; // z coordinate of the nodes' position (-1 = random)
        double minimumDistanceComputer = default(0); // Modified by Jorge Perez: minimum distance in m between any two computers in map
        double minimumDistanceAnchor = default(0); // Modified by Jorge Perez: minimum distance in m between any two anchors in map
        double minimumDistanceNode = default(0); // Modified by Jorge Perez: minimum distance in m between any two mobile nodes in map
        @display("i=block/cogwheel");
}

