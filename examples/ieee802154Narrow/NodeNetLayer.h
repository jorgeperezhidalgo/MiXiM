#include <BaseNetwLayer.h>
#include <NodeAppLayer.h>

class NodeNetLayer : public BaseNetwLayer
{
public:
    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);
    virtual void finish();

protected:

	/**
     * @name Handle Messages
     * @brief Functions to redefine by the programmer
     *
     * These are the functions provided to add own functionality to your
     * modules. These functions are called whenever a self message or a
     * data message from the upper or lower layer arrives respectively.
     *
     **/
    /*@{*/

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage* msg);

    /** @brief Handle messages from lower layer */
    virtual void handleLowerMsg(cMessage* msg);

    /** @brief Handle self messages */
    virtual void handleSelfMsg(cMessage* msg){
    	error("AnchorNetLayer does not generate self messages");
        };

    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage* msg);

    /** @brief Handle control messages from lower layer */
    virtual void handleUpperControl(cMessage* msg);

    /** @brief decapsulate higher layer message from NetwPkt */
    virtual cMessage* decapsMsg(NetwPkt*);

    /** @brief Encapsulate higher layer packet into an NetwPkt*/
    virtual NetwPkt* encapsMsg(cPacket*);
};
