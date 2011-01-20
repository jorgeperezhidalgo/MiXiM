/* -*-mode:c++ -*- *********************************************************
 * file:        StateVisualize.cc
 *
 * author:      Andreas Koepke
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     testsuite for framework
 * description: visualizes state subscribed from BB
 ***************************************************************************
 * changelog:   $Revision: 1.7 $
 *              last modified:   $Date: 2004/02/09 13:59:33 $
 *              by:              $Author: omfw-willkomm $
 **************************************************************************/


#include "StateVisualize.h"
#include "TestHostState.h"

Define_Module( StateVisualize );

void StateVisualize::initialize(int stage)
{
    BaseModule::initialize(stage);
    if(stage == 0) {
        TestHostState s;
        catHostState = utility->subscribe(this, &s);
    }
}

void StateVisualize::handleMessage( cMessage* m)
{
    error("StateVisualize::handleMessage called");
}

void StateVisualize::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{
    Enter_Method("receiveBBItem(%s)", details->info().c_str());
    if(category == catHostState) {
        const TestHostState *s = dynamic_cast<const TestHostState *>(details);
        if(s == 0)
            error("StateVisualize::handleMessage could not read value from Blackboard");

        if(s->getState() == TestHostState::DEAD) {
            ev << "StateVisualize::receiveBBItem "
               << "Host state changed to DEAD" << std::endl;
        }
        else if (s->getState() == TestHostState::SLEEP){
            ev << "StateVisualize::receiveBBItem "
               << "Host state changed to SLEEP" << std::endl;
        }
        else if (s->getState() == TestHostState::AWAKE){
            ev << "StateVisualize::receiveBBItem "
               << "Host state changed to AWAKE" << std::endl;
        } else {
            ev << "StateVisualize::receiveBBItem "
               << " Host changed to unkown state " << std::endl;
        }
    }
    else {
        error("StateVisualize::receiveBBItem called with wrong category");
    }
}

StateVisualize::~StateVisualize()
{

}
