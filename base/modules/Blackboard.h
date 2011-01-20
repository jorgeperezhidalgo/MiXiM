
#ifndef BLACK_BOARD_H
#define BLACK_BOARD_H

#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <omnetpp.h>
#include <vector>
#include <string>
#include "ImNotifiable.h"
/**
 * @brief Provides a blackboard like information distribution.
 *
 * In protocol simulations, one often has to evaluate the performance
 * of a protocol. This implies that not only the protocol has to be
 * developed, but also the code used for the performance evaluation.
 * Often, this leads to ugly implementations: real protocol code is
 * mixed with debug and performance evaluation code. This mix becomes
 * annoying if the code is made public to the community.
 *
 * Another, somewhat similar, problem appears if programmers want to
 * implement new protocols that integrate information from other
 * protocol layers. Usually, researchers have to re-implement at least
 * parts of the protocol to expose the necessary information -- making
 * it less useful for other researchers as this usually increases the
 * coupling of the protocols.
 *
 * One way around both problems is a black board. On the black board,
 * protocols publish their state and possible state changes. It is now
 * possible to separate performance monitors from the protocol
 * code. Also, cross-layer information exchange also boils down to
 * publishing and subscribing information -- without introducing tight
 * coupling. The only commonly known entity is the blackboard.
 *
 * The interaction with the blackboard is simple:
 *
 * publisher -publish(BBItem)-> Blackboard -notify(BBItem)--> subscriber
 *
 * The publisher can be anything that knows how to call a Blackboard
 * and how to construct a proper BBItem. It remains the logical owner
 * of the published item. The BB neither stores it, nor keeps any
 * reference to it.
 *
 * The subscriber must implement the ImNotifiable
 * interface. It can keep a copy of the published item (cache it), but
 * it should not keep a reference to it. Otherwise, things become
 * really messy. (Some code written for older versions of the BB
 * speaks for itself).
 *
 * @see ImNotifiable
 * @ingroup blackboard
 * @ingroup baseModules
 *
 * @author Andras Varga
 * @author Andreas Koepke
 */

class Blackboard : public cSimpleModule
{
protected:

	/** @brief Internal class to store subscriber data.*/
    class Subscriber {
    public:
    	/** @brief Pointer to the subscriber.*/
        ImNotifiable *client;
        /** @brief The scope for the subscriber.*/
        int scopeModuleId;
        public:
        /** @brief Inits this subscriber with the passed values.*/
        Subscriber(ImNotifiable *c=0, int b=-1) :
            client(c), scopeModuleId(b) {};
    };

    /** @name Typedefs for container shortcuts.*/
    /*@{*/
    typedef std::vector<Subscriber> SubscriberVector;
    typedef std::vector<SubscriberVector> ClientVector;

    typedef std::vector<const char* > CategoryDescriptions;

    typedef std::vector<int> ParentVector;

    typedef ClientVector::iterator ClientVectorIter;
    typedef CategoryDescriptions::iterator DescriptionsIter;
    typedef ParentVector::iterator ParentVectorIter;
    /*@}*/

    ClientVector clientVector;
    CategoryDescriptions categoryDescriptions;
    ParentVector parentVector;

    int nextCategory;

    friend std::ostream& operator<<(std::ostream&, const SubscriberVector&);

    bool coreDebug;

protected:

    /**
     * find the description of a category
     */
    const char* categoryName(int category);
    /**
     * find or create a category, returns iterator in map and sets
     * isNewEntry to true if the entry was created new.
     */
    int findAndCreateDescription(bool *isNewEntry, const BBItem *category);

    /**
     * traverse inheritance diagramm and make sure that children of
     * category are also delivered to modules that susbcribed to one
     * of its parent classes.
     */
    void fillParentVector(const BBItem *category, int cat);

 public:
    /** @brief This modules shouldn't receive any messages*/
    void handleMessage(cMessage *msg);

    /** @brief Initializes blackboard.*/
    virtual void initialize(int);

    Blackboard():
    	nextCategory(0)
	{}

    virtual ~Blackboard();


    /** @name Methods for consumers of change notifications */
    //@{
    /**
     * Subscribe to changes of a specific category. The category is
     * defined by the BBItem class to which this object belongs.
     * Returns the id of this category.
     *
     * Both subscribe functions subscribe also to children of this
     * category class.
     *
     */
    int subscribe(ImNotifiable *client, const BBItem *category, int scopeModuleId=-1);
    /**
     * Subscribe to changes of a specific category. This time using
     * the numeric id. This implies that you have previously called
     * subscribe with the appropriate category class.
     *
     * Both subscribe functions subscribe also to children of this
     * category class.
     *
     * Subscribe in stage 0 of initialize -- this way you get all
     * publishes that happen at stage 1.
     */
    int subscribe(ImNotifiable *client, int category, int scopeModuleId=-1);
    /**
     * Unsubscribe from notifications
     */
    void unsubscribe(ImNotifiable *client, int category);
    //@}

    /** @name Methods for producers of change notifications */
    //@{
    /**
     * Tells Blackboard that a change of the given category has taken
     * place. The optional details object (@see BBItem) may carry more
     * specific information about the change (e.g. exact location,
     * specific attribute that changed, old value, new value, etc).
     *
     * This function is very central once you start working with the
     * blackboard. You should heed the following advices:
     *
     *     - Publish in stage 1 of initialize after everbody was able
     *       to subscribe.
     *     - This function should be the last thing you call in a method.
     *       There is a chance that a certain race condition occurs,
     *       as in the following case, for simplicity the module is subscribed
     *       to the value that it publishes:
     *          -- Module::myMethod: lock module; publishBBItem; unlock module
     *          -- Module::receiveBBItem: if module not locked, do something
     *       Since receiveBBItem is called from within publishBBItem, the module
     *       will always be locked and the code in receiveBBItem is never
     *       executed.
     *
     */
    void publishBBItem(int category, const BBItem* details, int scopeModuleId);

    /**
     * Get the category from the BB -- if you don't want to subscribe but rather
     * publish
     */
    int  getCategory(const BBItem* details);
    //@}

    /** @brief We define two initialization stages.*/
    virtual int numInitStages() const {
		return 2;
	}
};

#endif

