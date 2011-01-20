#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <omnetpp.h>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <assert.h>
#include <FWMath.h>

#include "Interpolation.h"


/**
 * @brief Specifies a dimension for mappings (like time, frequency, etc.)
 *
 * The dimension is represented external by a string
 * (like "time") and internally by an unique ID.
 *
 * Note: Since the ID for a Dimensions is set the first time an instance
 * of this dimensions is created and the id is used to provide a
 * defined ordering of the Dimensions it DOES matter which
 * dimensions are instantiated the first time.
 * Only the time dimension will always have zero as unique id.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class Dimension {
protected:
	/** @brief Type for map from dimension name to ID.*/
	typedef std::map<std::string, int>DimensionIDMap;
	/** @brief Type for map from ID to dimension name.*/
	typedef std::map<int, std::string>DimensionNameMap;

	/**
	 * @brief stores the registered dimensions ids.
	 *
	 * Uses "construct-on-first-use" idiom to ensure correct initialization
	 * of static members.
	 */
	static DimensionIDMap& dimensionIDs();

	/**
	 * @brief ConstMapping from id to name of registered dimensions.
	 *
	 * Uses "construct-on-first-use" idiom to ensure correct initialization
	 * of static members.
	 */
	static DimensionNameMap& dimensionNames();

	/**
	 * @brief Stores the next free ID for a new dimension.
	 *
	 * Uses "construct-on-first-use" idiom to ensure correct initialization
	 * of static members.
	 */
	static int& nextFreeID();

	/**
	 * @brief Returns an instance of dimension which represents
	 * the dimension with the passed name.
	 */
	static int getDimensionID(const std::string& name);

	/** @brief The unique id of the dimension this instance represents.*/
	int id;

protected:

public:
	/** @brief Shortcut to the time Dimension, same as 'Dimension("time")',
	 * but spares the parsing of a string.*/
	static const Dimension time;

	/** @brief Shortcut to the frequency Dimension, same as 'Dimension("frequency")',
	 * but spares the parsing of a string.*/
	static const Dimension frequency;

public:
	Dimension():
		id(0)
	{}

	/**
	 * @brief Creates a new dimension instance representing the
	 * dimension with the passed name.
	 */
	Dimension(const std::string& name);

	/**
	 * @brief Shortcut to the time Dimension, same as 'Dimension("time")',
	 * but spares the parsing of a string.
	 *
	 * This method should be used instead of the static "time" member
	 * when used during initialization of static variables since it assures
	 * the correct order of initialization.
	 * */
	static Dimension& time_static();

	/**
	 * @brief Shortcut to the frequency Dimension, same as 'Dimension("frequency")',
	 * but spares the parsing of a string.
	 *
	 * This method should be used instead of the static "frequency" member
	 * when used during initialization of static variables since it assures
	 * the correct order of initialization.
	 */
	static Dimension& frequency_static();

	/**
	 * @brief Returns true if the ids of the two dimensions are equal.
	 */
	bool operator==(const Dimension& other) const;

	/**
	 * @brief Returns true if the id of the other dimension is
	 * greater then the id of this dimension.
	 *
	 * This is needed to be able to use Dimension as a key in std::map.
	 */
	bool operator<(const Dimension& other) const;

	/** @brief Sorting operator by dimension ID.*/
	bool operator<=(const Dimension& other) const { return id <= other.id; }
	/** @brief Sorting operator by dimension ID.*/
	bool operator>(const Dimension& other) const { return id > other.id; }
	/** @brief Sorting operator by dimension ID.*/
	bool operator>=(const Dimension& other) const { return id >= other.id; }
	/** @brief Sorting operator by dimension ID.*/
	bool operator!=(const Dimension& other) const { return id != other.id; }

	/**
	 * @brief Returns the name of this dimension.
	 */
	std::string getName() const{
		static DimensionNameMap& dimensionNames = Dimension::dimensionNames();
		return dimensionNames.find(id)->second;
	}

	/**
	 * @brief Returns the unique id of the dimension this instance
	 * represents.
	 *
	 * The id is used to uniquely identify dimensions as well as to
	 * provide a sorting of dimensions.
	 * Note: The "time"-dimension will always have the ID zero.
	 */
	int getID() const { return id; }

	/** @brief Output operator for a dimension.*/
	friend std::ostream& operator<<(std::ostream& out, const Dimension& d) {
    	return (out << d.getName() << "(" << d.id << ")");
    }
};

/**
 * @brief Represents a set of dimensions which is used to define over which
 * dimensions a mapping is defined (the domain of the mapping).
 *
 * This class actually public extends from std::set<Dimension>. So any method
 * provided by std::set can be also used.
 *
 * The dimensions are stored ordered by their ids.
 *
 * Note: Unlike Arguments and Mappings, a DimensionSet does not contain "time"
 * as dimension per default. You'll have to add it like any other dimension.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class DimensionSet:public std::set<Dimension> {
public:
	/** @brief Shortcut to a DimensionSet which only contains time. */
	static const DimensionSet timeDomain;

	/** @brief Shortcut to a DimensionSet which contains time and frequency. */
	static const DimensionSet timeFreqDomain;
public:
	/**
	 * @brief Default constructor creates an empty DimensionSet
	 */
	DimensionSet() {}

	/**
	 * @brief Creates a new DimensionSet with the passed Dimension as
	 * initial Dimension
	 */
	DimensionSet(const Dimension& d){
		this->insert(d);
	}
	/**
	 * @brief Creates a new DimensionSet with the passed Dimensions as
	 * initial Dimensions (convenience method)
	 */
	DimensionSet(const Dimension& d1, const Dimension& d2){
		this->insert(d1);
		this->insert(d2);
	}
	/**
	 * @brief Creates a new DimensionSet with the passed Dimensions as
	 * initial Dimensions (convenience method)
	 */
	DimensionSet(const Dimension& d1, const Dimension& d2, const Dimension& d3){
		this->insert(d1);
		this->insert(d2);
		this->insert(d3);
	}

	/**
	 * @brief Returns true if the passed DimensionSet is a subset
	 * of this DimensionSet.
	 *
	 * A DimensionSet is a subset of this DimensionSet if every
	 * Dimension of the passed DimensionSet is defined in this
	 * DimensionSet.
	 */
	bool isSubSet(const DimensionSet& other) const{
		if(this->size() < other.size())
			return false;

		return std::includes(this->begin(), this->end(), other.begin(), other.end());
	}

	/**
	 * @brief Returns true if the passed DimensionSet is a real subset
	 * of this DimensionSet.
	 *
	 * A DimensionSet is a real subset of this DimensionSet if every
	 * Dimension of the passed DimensionSet is defined in this
	 * DimensionSet and there is at least one Dimensions in this set
	 * which isn't in the other set.
	 */
	bool isRealSubSet(const DimensionSet& other) const{
		if(this->size() <= other.size())
			return false;

		return std::includes(this->begin(), this->end(), other.begin(), other.end());
	}

	/**
	 * @brief Adds the passed dimension to the DimensionSet.
	 */
	void addDimension(const Dimension& d) {
		this->insert(d);
	}

	/**
	 * @brief Returns true if the passed Dimension is inside this DimensionSet.
	 */
	bool hasDimension(const Dimension& d) const{
		return this->count(d) > 0;
	}

	/**
	 * @brief Returns true if the dimensions of both sets are equal.
	 */
	bool operator==(const DimensionSet& o){
		if(size() != o.size())
			return false;

		return std::equal(begin(), end(), o.begin());
	}
};

/**
 * @brief Defines an argument for a mapping.
 *
 * Defines values for a specified set of dimensions, but at
 * least for the time dimension.
 *
 * Note: Currently an Argument can be maximal defined over ten Dimensions
 * plus the time dimension!
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class Argument{
protected:

	/** @brief Stores the time dimension in Omnets time type */
	simtime_t time;

	/** @brief Maps the dimensions of this Argument to their values. */
	std::pair<Dimension, double> values[10];

	/** @brief The number of Dimensions this Argument has. */
	unsigned int count;

public:
	/** @brief Iterator type for this set.*/
	typedef std::pair<Dimension, double>* iterator;
	/** @brief Const-iterator type for this set.*/
	typedef const std::pair<Dimension, double>* const_iterator;

protected:
	/**
	 * @brief Inserts the passed value for the passed Dimension into
	 * this Argument.
	 *
	 * The parameter "pos" defines the position inside the Dimension<->Value-pair
	 * array to start searching for the dimension to set.
	 *
	 * If the "ignoreUnknown"-parameter is set to true the new value is only
	 * set if the Dimension was defined in this Argument before (means, no
	 * new DImensions are added to the Argument).
	 *
	 * The method returns the position inside the array the value was inserted.
	 */
	iterator insertValue(iterator pos, const Dimension& dim, double value, bool ignoreUnknown = false);

public:
	/**
	 * @brief Initialize this argument with the passed value for
	 * the time dimension.
	 */
	Argument(simtime_t timeVal = 0);

	/**
	 * @brief Initializes the Argument with the dimensions of the
	 * passed DimensionSet set to zero, and the passed value for the
	 * time (or zero, if omitted).
	 */
	Argument(const DimensionSet& dims, simtime_t timeVal = 0);

	/**
	 * @brief Returns the time value of this argument.
	 */
	simtime_t getTime() const;

	/**
	 * @brief Changes the time value of this argument.
	 */
	void setTime(simtime_t time);

	/**
	 * @brief Returns true if this Argument has a value for the
	 * passed Dimension.
	 */
	bool hasArgVal(const Dimension& dim) const;

	/**
	 * @brief Returns the value for the specified dimension.
	 *
	 * Note: Don't use this function to get the time value!
	 * Use "getTime()" instead.
	 *
	 * Returns zero if no value with the specified dimension
	 * is set for this argument.
	 */
	double getArgValue(const Dimension& dim) const;

	/**
	 * @brief Changes the value for the specified dimension.
	 *
	 * Note: Don't use this function to change the time value!
	 * Use "setTime()" instead.
	 *
	 * If the argument doesn't already contain a value for the
	 * specified dimension the new dimension is added.
	 */
	void setArgValue(const Dimension& dim, double value);

	/**
	 * @brief Update the values of this Argument with the values
	 * of the passed Argument.
	 *
	 * Only the dimensions from the passed Argument are updated or
	 * added.
	 *
	 * If the ignoreUnknown parameter is set to true, only the Dimensions
	 * already inside the Argument are updated.
	 */
	void setArgValues(const Argument& o, bool ignoreUnknown = false);

	/**
	 * @brief Returns true if the passed Argument points to
	 * the same position.
	 *
	 * The functions returns true if every Dimension in the passed
	 * Argument exists in this Argument and their values are
	 * the same. The difference to the == operator is that the
	 * dimensions of the passed Argument can be a subset of the
	 * dimensions of this Argument.
	 */
	bool isSamePosition(const Argument& other) const;

	/**
	 * @brief Two Arguments are compared equal if they have
	 * the same dimensions and the same values.
	 */
	bool operator==(const Argument& o) const;

	/**
	 * @brief Two Arguments are compared close if they have
	 * the same dimensions and their values don't differ more
	 * then a specific epsilon.
	 */
	bool isClose(const Argument& o, double epsilon = 0.000001) const;

	/**
	 * @brief Returns true if this Argument is smaller then the
	 * passed Argument. The dimensions of the Arguments have to
	 * be the same.
	 *
	 * An Argument is compared smaller than another Argument if
	 * the value of the Dimension with the highest id is compared
	 * smaller.
	 * If the value of the highest Dimension is compared bigger the
	 * Argument isn't compared smaller (method returns false).
	 * If the values of the Dimension with the highest Dimension are
	 * equal, the next smaller Dimension is compared.
	 */
	bool operator<(const Argument& o) const;

	/**
	 * @brief Compares this Argument with the passed Argument in the
	 * dimensions of the passed DimensionsSet. (Every other Dimension
	 * is asserted equal).
	 *
	 * @return < 0 - passed Argument is bigger
	 * 		   = 0 - Arguments are equal
	 * 		   > 0 - passed Argument is smaller
	 *
	 * See "operator<" for definition of smaller, equal and bigger.
	 */
	double compare(const Argument& o, const DimensionSet& dims) const;

	/**
	 * @brief Returns the dimensions this argument is defined over
	 *
	 * Note: this method has linear complexity over the number of dimensions,
	 * since the DimensionSet has to be created from the values and their
	 * dimensions inside this Argument.
	 */
	DimensionSet getDimensions() const {
		DimensionSet res(Dimension::time_static());

		for(unsigned int i = 0; i < count; i++)
			res.addDimension(values[i].first);

		return res;
	}

	/**
	 * @brief Output operator for Arguments.
	 *
	 * Produces output of form "(x1, x2, x3, <...>)".
	 */
	friend std::ostream& operator<<(std::ostream& out, const Argument& d) {
		out << "(" << d.time;

		for(unsigned int i = 0; i < d.count; i++){
			out << ", " << d.values[i].second;
		}
    	return (out << ")");
    }

	/**
	 * @brief Fast implementation of the copy-operator then the default
	 * implementation.
	 */
	void operator=(const Argument& o);

	/**
	 * @brief Returns an iterator to the first argument value in this Argument.
	 */
	iterator begin() { return values; }
	/**
	 * @brief Returns an iterator to the first argument value in this Argument.
	 */
	const_iterator begin() const { return values; }


	/**
	 * @brief Returns an iterator to the value behind the last argument value.
	 */
	iterator end() { return values + count; }
	/**
	 * @brief Returns an iterator to the value behind the last argument value.
	 */
	const_iterator end() const { return values + count; }

	/**
	 * @brief Returns an iterator to the Argument value for the passed Dimension.
	 *
	 * Returns end() if there is no Argument for that dimension.
	 */
	iterator find(const Dimension& dim);
	/**
	 * @brief Returns an iterator to the Argument value for the passed Dimension.
	 *
	 * Returns end() if there is no Argument for that dimension.
	 */
	const_iterator find(const Dimension& dim) const;

	/**
	 * @brief Returns an iterator to the first Argument value which dimension
	 * compares greater or equal to the passed Dimension.
	 */
	iterator lower_bound(const Dimension& dim);
	/**
	 * @brief Returns an iterator to the first Argument value which dimension
	 * compares greater or equal to the passed Dimension.
	 */
	const_iterator lower_bound(const Dimension& dim) const;
};

/**
 * @brief This exception is thrown by the MappingIterators when "next()" or "nextPosition()" is called
 * although "hasNext()" would return false (means there is no next position).
 *
 * Although this exception isn't thrown by every implementation of the "next()"-method it is always
 * a bad idea to call "next()" although there isn't any next position.
 * You should check "hasNext()" before calling "next()" or "nextPosition()".
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class NoNextIteratorException {};

/**
 * @brief Defines an const iterator for a ConstMapping which is able
 * to iterate over the Mapping.
 *
 * Iterators provide a fast way to iterate over every "entry" or important
 * position of a Mapping. They also provide constant complexity for accessing
 * the Mapping at the position of the Iterator.
 *
 * Every implementation of an mapping-Iterator provides the same ordering
 * when iterating over a Mapping. This is that, if the iterator is increased,
 * the position of the entry it points to has to be also "increased". Which
 * means that the new position has to be compared bigger than the previous
 * position (see class Argument for comparison of positions).
 *
 * "Const" means that you can not change the values of the underlying Mapping
 * with this iterator.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class ConstMappingIterator {
public:
	virtual ~ConstMappingIterator() {}

	/**
	 * @brief Returns the position the next call to "next()" of this
	 * Iterator would iterate to.
	 */
	virtual const Argument& getNextPosition() const = 0;

	/**
	 * @brief Lets the iterator point to the passed position.
	 *
	 * The passed new position can be at arbitrary places.
	 */
	virtual void jumpTo(const Argument& pos) = 0;

	/**
	 * @brief Lets the iterator point to the begin of the mapping.
	 *
	 * The beginning of the mapping depends on the implementation. With an
	 * implementation based on a number of key-entries, this could be the
	 * key entry with the smallest position (see class Argument for ordering
	 * of positions).
	 */
	virtual void jumpToBegin() = 0;

	/**
	 * @brief Iterates to the specified position. This method
	 * should be used if the new position is near the current position.
	 * Furthermore the new position has to be compared bigger than
	 * the old position.
	 */
	virtual void iterateTo(const Argument& pos) = 0;

	/**
	 * @brief Iterates to the next position of the Mapping.
	 *
	 * The next position depends on the implementation of the
	 * Mapping. With an implementation based on a number of key-entries
	 * this probably would be the next bigger key-entry.
	 */
	virtual void next() = 0;

	/**
	 * @brief Returns true if the current position of the iterator
	 * is in range of the function.
	 *
	 * This method should be used as end-condition when iterating
	 * over the function with the "next()" method.
	 */
	virtual bool inRange() const = 0;

	/**
	 * @brief Returns true if the iterator has a next value it
	 * can iterate to on a call to "next()".
	 */
	virtual bool hasNext() const = 0;

	/**
	 * @brief Returns the current position of the iterator.
	 */
	virtual const Argument& getPosition() const = 0;

	/**
	 * @brief Returns the value of the Mapping at the current
	 * position.
	 *
	 * The complexity of this method should be constant for every
	 * implementation.
	 */
	virtual double getValue() const = 0;
};

class Mapping;

/**
 * @brief Represents a not changeable mapping (mathematical function)
 * from domain with at least the time to  a double value.
 *
 * This class is an interface which describes a mapping (math.)
 * from a arbitrary dimensional domain (represented by a DimensionSet)
 * to a double value.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class ConstMapping {
protected:
	/** @brief The dimensions of this mappings domain.*/
	DimensionSet dimensions;

private:
	template<class T>
	std::string toString(T v, unsigned int length){
		char* tmp = new char[255];
		sprintf(tmp, "%.2f", v);

		std::string result(tmp);
		delete[] tmp;
		while(result.length() < length)
			result += " ";
		return result;
	}

	std::string toString(simtime_t v, unsigned int length){
		return toString(SIMTIME_DBL(v), length);
	}

public:

	/**
	 * @brief Initializes the ConstMapping with a the time dimension as domain.
	 */
	ConstMapping():
		dimensions(Dimension::time_static()) {}

	/**
	 * @brief Initializes the ConstMapping with the passed DimensionSet as
	 * Domain.
	 *
	 * The passed DimensionSet has to contain the time dimension!
	 */
	ConstMapping(const DimensionSet& dimSet):
		dimensions(dimSet) {

		assert(dimSet.hasDimension(Dimension::time_static()));
	}

	virtual ~ConstMapping() {}

	/**
	 * @brief Returns the value of this Mapping at the position specified
	 * by the passed Argument.
	 *
	 * The complexity of this method depends on the actual implementation.
	 */
	virtual double getValue(const Argument& pos) const = 0;

	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over this Mapping.
	 *
	 * See class ConstIterator for details.
	 */
	virtual ConstMappingIterator* createConstIterator() = 0;

	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over the function. The iterator starts at the passed position.
	 *
	 * See class ConstIterator for details.
	 */
	virtual ConstMappingIterator* createConstIterator(const Argument& pos) = 0;

	/**
	 * @brief returns a deep copy of this mapping instance.
	 */
	virtual ConstMapping* constClone() const = 0;

	/**
	 * @brief Returns the value of this Mapping at the position specified
	 * by the passed Argument.
	 */
	double operator[](const Argument& pos) const {
		return getValue(pos);
	}

	/**
	 * @brief Returns this Mappings domain as DimensionSet.
	 */
	const DimensionSet& getDimensionSet() const {
		return dimensions;
	}

	/**
	 * @brief Prints the Mapping to an output stream.
	 *
	 * This implementation is still pretty ugly, but it does
	 * the job.
	 */
	template<class stream>
	void print(stream& out) {
		ConstMapping& m = *this;
		Dimension otherDim;
		const DimensionSet& dims = m.getDimensionSet();
		out << "Mapping domain: time";
		for(DimensionSet::iterator it = dims.begin(); it != dims.end(); ++it) {
			if(*it != Dimension::time){
				otherDim = *it;
				out << ", " << *it;
			}
		}
		out << endl;

		ConstMappingIterator* it = m.createConstIterator();

		if(!it->inRange()) {
			out << "Mapping is empty." << endl;
			return;
		}

		Argument min = it->getPosition();
		Argument max = it->getPosition();

		std::set<simtime_t> timePositions;
		std::set<double> otherPositions;

		timePositions.insert(it->getPosition().getTime());
		if(dims.size() == 2){
			otherPositions.insert(it->getPosition().begin()->second);
		}

		while(it->hasNext()) {
			it->next();
			const Argument& pos = it->getPosition();

			min.setTime(std::min(min.getTime(), pos.getTime()));
			max.setTime(std::max(max.getTime(), pos.getTime()));

			timePositions.insert(pos.getTime());

			for(Argument::const_iterator itA = pos.begin(); itA != pos.end(); ++itA) {
				if(dims.size() == 2){
					otherPositions.insert(itA->second);
				}
				min.setArgValue(itA->first, std::min(min.getArgValue(itA->first), itA->second));
				max.setArgValue(itA->first, std::max(max.getArgValue(itA->first), itA->second));
			}
		}
		delete it;

		if(dims.size() > 2) {
			out << "domain - min=" << min << " max=" << max << endl;
			return;
		}

		out << "------+---------------------------------------------------------" << endl;
		out << "o\\t   | ";
		std::set<simtime_t>::const_iterator tIt;
		for(tIt = timePositions.begin();
			tIt != timePositions.end(); ++tIt)
		{
			out << m.toString(*tIt * 1000, 6) << " ";
		}
		out << endl;
		out << "------+---------------------------------------------------------" << endl;

		it = m.createConstIterator();


		if(dims.size() == 1) {
			out << "value" << " | ";
			while(it->inRange()) {
				out << m.toString(FWMath::mW2dBm(it->getValue()), 6) << " ";

				if(!it->hasNext()) {
					break;
				}
				it->next();
			}
		} else {
			tIt = timePositions.begin();
			std::set<double>::const_iterator fIt = otherPositions.begin();
			out << m.toString(*fIt, 5) << " | ";
			while(it->inRange()) {
				if(*fIt != it->getPosition().getArgValue(otherDim)) {
					++fIt;
					out << endl << m.toString(*fIt, 5) << " | ";
					tIt = timePositions.begin();
					assert(*fIt == it->getPosition().getArgValue(otherDim));
				}

				while(*tIt < it->getPosition().getTime()){
					++tIt;
					out << "      ";
				}

				out << m.toString(FWMath::mW2dBm(it->getValue()), 6) << " ";

				if(!it->hasNext()) {
					break;
				}
				it->next();
			}
		}
		out << endl << "------+---------------------------------------------------------" << endl;

	}
};


/**
 * @brief Defines an iterator for a Mapping which is able
 * to iterate over the Mapping.
 *
 * See class ConstMapping for details on Mapping iterators.
 *
 * Implementations of this class are able to change the underlying
 * Mapping at the current position of the iterator with constant
 * complexity.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class MappingIterator:public ConstMappingIterator {

public:
	virtual ~MappingIterator() {}
	/**
	 * @brief Changes the value of the Mapping at the current
	 * position.
	 *
	 * Implementations of this method should provide constant
	 * complexity.
	 */
	virtual void setValue(double value) = 0;
};


/**
 * @brief Represents a changeable mapping (mathematical function)
 * from at least time to double.
 *
 * This class extends the ConstMapping interface with write access.
 *
 * See ConstMapping for details.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class Mapping:public ConstMapping {
public:
	/** @brief Types of interpolation methods for mappings.*/
	enum InterpolationMethod {
		/** @brief interpolates with next lower entry*/
		STEPS,
		/** @brief interpolates with nearest entry*/
		NEAREST,
		/** @brief interpolates linear with next lower and next upper entry
				   constant before the first and after the last entry*/
		LINEAR
	};

protected:

public:

	/**
	 * @brief Initializes the Mapping with the passed DimensionSet as domain.
	 *
	 * The passed DimensionSet has to contain the time dimension!
	 */
	Mapping(const DimensionSet& dims):
		ConstMapping(dims) {}

	/**
	 * @brief Initializes the Mapping with the time dimension as domain.
	 */
	Mapping():
			ConstMapping() {}

	virtual ~Mapping() {}

	/**
	 * @brief Changes the value of the Mapping at the specified
	 * position.
	 *
	 * The complexity of this method depends on the implementation.
	 */
	virtual void setValue(const Argument& pos, double value) = 0;

	/**
	 * @brief Appends the passed value at the passed position to the mapping.
	 * This method assumes that the passed position is after the last key entry
	 * of the mapping.
	 *
	 * Depending on the implementation of the underlying method this method
	 * could be faster or at least as fast as the "setValue()"-method.
	 *
	 * Implementations of the Mapping interface can override this method
	 * if appending values to the end of the mapping could be implemented
	 * faster than the "setValue()" method. Otherwise this method just
	 * calls the "setValue()"-method implementation.
	 */
	virtual void appendValue(const Argument& pos, double value) {
		setValue(pos, value);
	}

	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over the Mapping and can change it.
	 */
	virtual MappingIterator* createIterator() = 0;

	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over the function and can change it.
	 *
	 * The iterator starts at the passed position.
	 */
	virtual MappingIterator* createIterator(const Argument& pos) = 0;

	/**
	 * @brief Returns an ConstMappingIterator by use of the respective implementation
	 * of the "createIterator()"-method.
	 *
	 * Override this method if your ConstIterator differs from the normal iterator.
	 */
	virtual ConstMappingIterator* createConstIterator() {
		return createIterator();
	}

	/**
	 * @brief Returns an ConstMappingIterator by use of the respective implementation
	 * of the "createIterator()"-method.
	 *
	 * Override this method if your ConstIterator differs from the normal iterator.
	 */
	virtual ConstMappingIterator* createConstIterator(const Argument& pos) {
		return createIterator(pos);
	}



	/**
	 * @brief Returns a deep copy of this Mapping.
	 */
	virtual Mapping* clone() const = 0;

	/**
	 * @brief Returns a deep const copy of this mapping by using
	 * the according "clone()"-implementation.
	 */
	virtual ConstMapping* constClone() const { return clone(); }


};


//###################################################################################
//#                     default Mapping implementations                              #
//###################################################################################

/**
 * @brief A fully working ConstIterator-implementation usable with almost every
 * ConstMapping.
 *
 * Although this ConstIterator would work with almost any ConstMapping it should
 * only be used for ConstMappings whose "getValue()"-method has constant complexity.
 * This is because the iterator just calls the "getValue()"-method of the
 * underlying ConstMapping on every call of its own "getValue()"-method.
 *
 * The underlying ConstMapping has to provide a set of key-entries (Arguments) to the
 * iterator to tell it the positions it should iterate over.
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class SimpleConstMappingIterator:public ConstMappingIterator {
protected:
	/** @brief The underlying ConstMapping to iterate over. */
	ConstMapping* mapping;

	/** @brief The dimensions of the underlying ConstMapping.*/
	DimensionSet dimensions;

	/** @brief The current position of the iterator.*/
	Argument position;

	/** @brief Type for a set of Arguments defining key entries.*/
	typedef std::set<Argument> KeyEntrySet;

	/** @brief A pointer to a set of Arguments defining the positions to
	 * iterate over.
	 */
	const KeyEntrySet* keyEntries;

	/** @brief An iterator over the key entry set which defines the next bigger
	 * entry of the current position.*/
	KeyEntrySet::const_iterator nextEntry;

public:
	/**
	 * @brief Initializes the ConstIterator for the passed ConstMapping,
	 * with the passed key entries to iterate over and the passed position
	 * as start.
	 *
	 * Note: The pointer to the key entries has to be valid as long as the
	 * iterator exists.
	 */
	SimpleConstMappingIterator(ConstMapping* mapping,
							   const std::set<Argument>* keyEntries,
							   const Argument& start);

	/**
	 * @brief Initializes the ConstIterator for the passed ConstMapping,
	 * with the passed key entries to iterate over.
	 *
	 * Note: The pointer to the key entries has to be valid as long as the
	 * iterator exists.
	 */
	SimpleConstMappingIterator(ConstMapping* mapping,
							   const std::set<Argument>* keyEntries);

	/**
	 * @brief Returns the next position a call to "next()" would iterate to.
	 *
	 * This method has constant complexity.
	 *
	 * Throws an NoNextIteratorException if there is no next point of interest.
	 */
	virtual const Argument& getNextPosition() const {
		if(nextEntry == keyEntries->end())
			throw NoNextIteratorException();

		return *nextEntry;
	}

	/**
	 * @brief Lets the iterator point to the passed position.
	 *
	 * This method has logarithmic complexity (over the number of key entries).
	 */
	virtual void jumpTo(const Argument& pos) {
		position.setArgValues(pos, true);
		nextEntry = keyEntries->upper_bound(position);
	}

	/**
	 * @brief Lets the iterator point to the first "position of interest"
	 * of the underlying mapping.
	 *
	 * This method has constant complexity.
	 */
	virtual void jumpToBegin(){
		nextEntry = keyEntries->begin();

		if(nextEntry == keyEntries->end()) {
			position = Argument(dimensions);
		} else {
			position = *nextEntry;
			++nextEntry;
		}
	}

	/**
	 * @brief Increases the position of the iterator to the passed
	 * position.
	 *
	 * The passed position has to be compared greater than the previous
	 * position.
	 *
	 * This method has constant complexity.
	 */
	virtual void iterateTo(const Argument& pos) {
		position.setArgValues(pos, true);
		while(nextEntry != keyEntries->end() && !(position < *nextEntry))
			++nextEntry;

	}

	/**
	 * @brief Iterates to the next "point of interest" of the Mapping.
	 *
	 * Throws an NoNextIteratorException if there is no next point of interest.
	 *
	 * This method has constant complexity.
	 */
	virtual void next() {
		if(nextEntry == keyEntries->end())
			throw NoNextIteratorException();

		position = *nextEntry;
		++nextEntry;
	}

	/**
	 * @brief Returns true if the current position of the iterator is equal or bigger
	 * than the first point of interest and lower or equal than the last point of
	 * interest.
	 *
	 * Has constant complexity.
	 */
	virtual bool inRange() const {
		return !(keyEntries->empty()
				 || (*(keyEntries->rbegin()) < position)
				 || (position < *(keyEntries->begin())));
	}

	/**
	 * @brief Returns true if there is a next position a call to "next()" can iterate to.
	 *
	 * Has constant complexity.
	 */
	virtual bool hasNext() const { return nextEntry != keyEntries->end(); }

	/**
	 * @brief Returns the current position of the iterator.
	 *
	 * Constant complexity.
	 */
	virtual const Argument& getPosition() const { return position; }

	/**
	 * @brief Returns the value of the underlying mapping at the current
	 * position of the iterator.
	 *
	 * This method has the same complexity as the "getValue()" method of the
	 * underlying mapping.
	 */
	virtual double getValue() const { return mapping->getValue(position); }
};

/**
 * @brief Abstract subclass of ConstMapping which can be used as base for
 * any ConstMapping implementation with read access of constant complexity.
 *
 * Any subclass only has to implement the "getValue()" and the "clone()"-method.
 *
 * The subclass has to define the "points of interest" an iterator should
 * iterate over.
 * This should be done either at construction time or later by calling the
 * "initializeArguments()"-method.
 *
 * The SimpleConstMapping class provides Iterator creation by using the
 * SimpleConstMappingIterator which assumes that the underlying ConstMappings
 * getValue()-method is fast enough to be called on every iteration step
 * (which means constant complexity).
 *
 * @author Karl Wessel
 * @ingroup mapping
 */
class SimpleConstMapping:public ConstMapping {
protected:
	/** @brief Type for a set of Arguments defining key entries.*/
	typedef std::set<Argument> KeyEntrySet;

	/** @brief A set of Arguments defining the "points of interest" an iterator
	 * should iterate over.*/
	KeyEntrySet keyEntries;

protected:

	/**
	 * @brief Utility method to fill add range of key entries in the time dimension
	 * to the key entry set.
	 */
	void createKeyEntries(const Argument& from, const Argument& to, const Argument& step, Argument& pos);

	/**
	 * @brief Utility method to fill add range of key entries in the passed dimension
	 * (and recursively its sub dimensions) to the key entry set.
	 */
	void createKeyEntries(const Argument& from, const Argument& to, const Argument& step,
						  DimensionSet::const_iterator curDim, Argument& pos);

public:
	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over the Mapping.
	 *
	 * This method asserts that the mapping had been fully initialized.
	 */
	virtual ConstMappingIterator* createConstIterator() {
		return new SimpleConstMappingIterator(this, &keyEntries);
	}

	/**
	 * @brief Returns a pointer of a new Iterator which is able to iterate
	 * over the Mapping. The iterator starts at the passed position.
	 *
	 * This method asserts that the mapping had been fully initialized.
	 */
	virtual ConstMappingIterator* createConstIterator(const Argument& pos) {
		return new SimpleConstMappingIterator(this, &keyEntries, pos);
	}

public:
	/**
	 * @brief Initializes a not yet iterateable SimpleConstmapping with the
	 * passed DimensionSet as domain.
	 */
	SimpleConstMapping(const DimensionSet& dims):
		ConstMapping(dims) {}

	/**
	 * @brief Fully initializes this mapping with the passed position
	 * as key entry.
	 *
	 * A SimpleConstMapping initialized by this constructor is able to return a valid
	 * ConstIterator.
	 */
	SimpleConstMapping(const DimensionSet& dims,
					   const Argument& key):
		ConstMapping(dims){

		initializeArguments(key);
	}

	/**
	 * @brief Fully initializes this mapping with the passed two positions
	 * as key entries.
	 *
	 * A SimpleConstMapping initialized by this constructor is able to return a valid
	 * ConstIterator.
	 */
	SimpleConstMapping(const DimensionSet& dims,
					   const Argument& key1, const Argument& key2):
		ConstMapping(dims){

		initializeArguments(key1, key2);
	}

	/**
	 * @brief Fully initializes this mapping with the key entries defined by
	 * the passed min, max and interval values.
	 *
	 * A SimpleConstMapping initialized by this constructor is able to return a valid
	 * ConstIterator.
	 */
	SimpleConstMapping(const DimensionSet& dims,
					   const Argument& min, const Argument& max, const Argument& interval):
		ConstMapping(dims){

		initializeArguments(min, max, interval);
	}

	/**
	 * @brief Initializes the key entry set with the passed position
	 * as entry.
	 *
	 * Convenience method for simple mappings.
	 */
	void initializeArguments(const Argument& key){
		keyEntries.clear();
		keyEntries.insert(key);
	}

	/**
	 * @brief Initializes the key entry set with the passed two positions
	 * as entries.
	 *
	 * Convenience method for simple mappings.
	 */
	void initializeArguments(const Argument& key1, const Argument& key2){
		keyEntries.clear();
		keyEntries.insert(key1);
		keyEntries.insert(key2);
	}

	/**
	 * @brief Initializes the key entry set with the passed min, max and
	 * interval-Arguments.
	 *
	 * After a call to this method this SimpleConstMapping is able to return a valid
	 * ConstIterator.
	 */
	void initializeArguments(const Argument& min,
							 const Argument& max,
							 const Argument& interval);

	/**
	 * @brief Returns the value of the mapping at the passed position.
	 *
	 * This method has to be implemented by every subclass and should
	 * only have constant complexity.
	 */
	virtual double getValue(const Argument& pos) const = 0;

	/**
	 * @brief creates a clone of this mapping.
	 *
	 * This method has to be implemented by every subclass.
	 */
	ConstMapping* constClone() const  = 0;
};




#endif /*FUNCTION_H_*/
