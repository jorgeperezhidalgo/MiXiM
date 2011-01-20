/*
 * MappingUtils.cc
 *
 *  Created on: 26.08.2008
 *      Author: Karl Wessel
 */

#include "MappingUtils.h"


FilledUpMappingIterator::FilledUpMappingIterator(FilledUpMapping& mapping):
	MultiDimMappingIterator<Linear>(mapping) {}

FilledUpMappingIterator::FilledUpMappingIterator(FilledUpMapping& mapping, const Argument& pos):
	MultiDimMappingIterator<Linear>(mapping, pos) {}



MappingUtils::MappingBuffer MappingUtils::mappingBuffer;

ConstMapping* MappingUtils::createCompatibleMapping(ConstMapping& src, ConstMapping& dst){
	typedef FilledUpMapping::KeySet KeySet;
	typedef FilledUpMapping::KeyMap KeyMap;
	KeyMap keys;

	const DimensionSet& srcDims = src.getDimensionSet();
	const DimensionSet& dstDims = dst.getDimensionSet();

	DimensionSet::const_reverse_iterator srcDimIt = srcDims.rbegin();
	for (DimensionSet::const_reverse_iterator dstDimIt = dstDims.rbegin();
		 dstDimIt != dstDims.rend(); ++dstDimIt)
	{
		if(srcDimIt != srcDims.rend() && *srcDimIt == *dstDimIt){
			++srcDimIt;
		} else {
			keys.insert(keys.end(),
						KeyMap::value_type(*dstDimIt,
											KeySet()));
		}
	}

	if(keys.empty())
		return &src;

	ConstMappingIterator* dstIt = dst.createConstIterator();

	if(!dstIt->inRange()){
		delete dstIt;
		return &src;
	}

	do{
		for (KeyMap::iterator keyDimIt = keys.begin();
			 keyDimIt != keys.end(); ++keyDimIt)
		{
			keyDimIt->second.insert(dstIt->getPosition().getArgValue(keyDimIt->first));
		}

		if(!dstIt->hasNext())
			break;

		dstIt->next();
	}while(true);

	delete dstIt;

	ConstMapping* res = setMappingBuffer(new FilledUpMapping(&src, dstDims, &keys));
	return res;
}

bool MappingUtils::iterateToNext(ConstMappingIterator* it1, ConstMappingIterator* it2){
	bool it1HasNext = it1->hasNext();
	bool it2HasNext = it2->hasNext();

	if(it1HasNext || it2HasNext){
		if(it1HasNext && (!it2HasNext || it1->getNextPosition() < it2->getNextPosition())){
			it1->next();
			it2->iterateTo(it1->getPosition());

		} else {
			it2->next();
			it1->iterateTo(it2->getPosition());
		}

		return true;
	} else {
		return false;
	}
}

Mapping* MappingUtils::createMapping(const DimensionSet& domain,
								  Mapping::InterpolationMethod intpl) {
	assert(domain.hasDimension(Dimension::time));

	if(domain.size() == 1){
		switch(intpl){
		case Mapping::LINEAR:
			return new TimeMapping<Linear>(intpl);
			break;
		case Mapping::NEAREST:
			return new TimeMapping<Nearest>(intpl);
			break;
		case Mapping::STEPS:
			return new TimeMapping<NextSmaller>(intpl);
			break;
		}
		return 0;
	} else {
		switch(intpl){
		case Mapping::LINEAR:
			return new MultiDimMapping<Linear>(domain, intpl);
			break;
		case Mapping::NEAREST:
			return new MultiDimMapping<Nearest>(domain, intpl);
			break;
		case Mapping::STEPS:
			return new MultiDimMapping<NextSmaller>(domain, intpl);
			break;
		}
		return 0;
	}
}

Mapping* MappingUtils::createMapping(double outOfRangeVal,
								const DimensionSet& domain,
								Mapping::InterpolationMethod intpl) {
	assert(domain.hasDimension(Dimension::time));

	if(domain.size() == 1){
		switch(intpl){
		case Mapping::LINEAR:
			return new TimeMapping<Linear>(outOfRangeVal, intpl);
			break;
		case Mapping::NEAREST:
			return new TimeMapping<Nearest>(outOfRangeVal, intpl);
			break;
		case Mapping::STEPS:
			return new TimeMapping<NextSmaller>(outOfRangeVal, intpl);
			break;
		}
		return 0;
	} else {
		switch(intpl){
		case Mapping::LINEAR:
			return new MultiDimMapping<Linear>(domain, outOfRangeVal, intpl);
			break;
		case Mapping::NEAREST:
			return new MultiDimMapping<Nearest>(domain, outOfRangeVal, intpl);
			break;
		case Mapping::STEPS:
			return new MultiDimMapping<NextSmaller>(domain, outOfRangeVal, intpl);
			break;
		}
		return 0;
	}
}

Mapping* MappingUtils::multiply(ConstMapping &f1, ConstMapping &f2)
{
	return applyElementWiseOperator(f1, f2, std::multiplies<double>());
}

Mapping* MappingUtils::divide(ConstMapping &f1, ConstMapping &f2)
{
	return applyElementWiseOperator(f1, f2, std::divides<double>());
}

Mapping* MappingUtils::add(ConstMapping &f1, ConstMapping &f2)
{
	return applyElementWiseOperator(f1, f2, std::plus<double>());
}

Mapping* MappingUtils::subtract(ConstMapping &f1, ConstMapping &f2)
{
	return applyElementWiseOperator(f1, f2, std::minus<double>());
}



Mapping* MappingUtils::multiply(ConstMapping &f1, ConstMapping &f2, double outOfRangeVal)
{
	return applyElementWiseOperator(f1, f2, std::multiplies<double>(), outOfRangeVal, false);
}

Mapping* MappingUtils::divide(ConstMapping &f1, ConstMapping &f2, double outOfRangeVal)
{
	return applyElementWiseOperator(f1, f2, std::divides<double>(), outOfRangeVal, false);
}

Mapping* MappingUtils::add(ConstMapping &f1, ConstMapping &f2, double outOfRangeVal)
{
	return applyElementWiseOperator(f1, f2, std::plus<double>(), outOfRangeVal, false);
}

Mapping* MappingUtils::subtract(ConstMapping &f1, ConstMapping &f2, double outOfRangeVal)
{
	return applyElementWiseOperator(f1, f2, std::minus<double>(), outOfRangeVal, false);
}



Mapping* operator*(ConstMapping& f1, ConstMapping& f2) {
	return MappingUtils::multiply(f1, f2);
}

Mapping* operator/(ConstMapping& f1, ConstMapping& f2) {
	return MappingUtils::divide(f1, f2);
}

Mapping* operator+(ConstMapping& f1, ConstMapping& f2) {
	return MappingUtils::add(f1, f2);
}

Mapping* operator-(ConstMapping& f1, ConstMapping& f2) {
	return MappingUtils::subtract(f1, f2);
}


double MappingUtils::findMax(ConstMapping& m) {
	ConstMappingIterator* it = m.createConstIterator();

	double res = -DBL_MAX;

	while(it->inRange()){
		double val = it->getValue();
		if(val > res)
			res = val;

		if(!it->hasNext())
			break;

		it->next();
	}
	delete it;
	return res;
}

double MappingUtils::findMax(ConstMapping& m, const Argument& min, const Argument& max){
	//the passed interval should define a value for every dimension
	//of the mapping.
	assert(min.getDimensions().isSubSet(m.getDimensionSet()));
	assert(max.getDimensions().isSubSet(m.getDimensionSet()));

	ConstMappingIterator* it = m.createConstIterator(min);

	double res = it->getValue();

	while(it->hasNext() && it->getNextPosition().compare(max, m.getDimensionSet()) < 0){
		it->next();

		const Argument& next = it->getPosition();
		bool inRange = next.getTime() >= min.getTime() && next.getTime() <= max.getTime();
		if(inRange) {
			for(Argument::const_iterator itA = next.begin(); itA != next.end(); ++itA) {
				if(itA->second < min.getArgValue(itA->first) || itA->second > max.getArgValue(itA->first)) {
					inRange = false;
					break;
				}
			}
		}

		if(inRange) {
			double val = it->getValue();
			if(val > res)
				res = val;
		}
	}
	it->iterateTo(max);
	double val = it->getValue();
	if(val > res)
		res = val;

	delete it;
	return res;
}

double MappingUtils::findMin(ConstMapping& m) {
	ConstMappingIterator* it = m.createConstIterator();

	double res = DBL_MAX;

	while(it->inRange()){
		double val = it->getValue();
		if(val < res)
			res = val;

		if(!it->hasNext())
			break;

		it->next();
	}
	delete it;
	return res;
}

double MappingUtils::findMin(ConstMapping& m, const Argument& min, const Argument& max){

	//the passed interval should define a value for every dimension
	//of the mapping.
	assert(min.getDimensions().isSubSet(m.getDimensionSet()));
	assert(max.getDimensions().isSubSet(m.getDimensionSet()));

	ConstMappingIterator* it = m.createConstIterator(min);

	double res = it->getValue();

	while(it->hasNext() && it->getNextPosition().compare(max, m.getDimensionSet()) < 0){
		it->next();

		const Argument& next = it->getPosition();
		bool inRange = next.getTime() >= min.getTime() && next.getTime() <= max.getTime();
		if(inRange) {
			for(Argument::const_iterator itA = next.begin(); itA != next.end(); ++itA) {
				if(itA->second < min.getArgValue(itA->first) || itA->second > max.getArgValue(itA->first)) {
					inRange = false;
					break;
				}
			}
		}

		if(inRange) {
			double val = it->getValue();
			if(val < res)
				res = val;
		}
	}
	it->iterateTo(max);
	double val = it->getValue();
	if(val < res)
		res = val;
	delete it;
	return res;
}


void MappingUtils::addDiscontinuity(Mapping* m,
									const Argument& pos, double value,
									simtime_t limitTime, double limitValue)
{
	// asserts/preconditions
	// make sure the time really differs at the discontinuity
	assert(limitTime != pos.getTime());

	// add (pos, value) to mapping
	m->setValue(pos, value);

	// create Argument limitPos for the limit-position, i.e. copy pos and set limitTime as its time
	Argument limitPos = pos;
	limitPos.setTime(limitTime);

	// add (limitPos, limitValue) to mapping
	m->setValue(limitPos, limitValue);
}

simtime_t MappingUtils::pre(simtime_t t)
{
	t.setRaw(t.raw() - 1);

	return t;
}

simtime_t MappingUtils::post(simtime_t t)
{
	assert(t.raw() < simtime_t::getMaxTime().raw());

	t.setRaw(t.raw() + 1);

	return t;
}


/*
Mapping* Mapping::multiply(ConstMapping &f1, ConstMapping &f2, const Argument& from, const Argument& to)
{
	return applyElementWiseOperator(f1, f2, std::multiplies<double>());
}

Mapping* Mapping::divide(ConstMapping &f1, ConstMapping &f2, const Argument& from, const Argument& to)
{
	return applyElementWiseOperator(f1, f2, std::divides<double>());
}

Mapping* Mapping::add(ConstMapping &f1, ConstMapping &f2, const Argument& from, const Argument& to)
{
	return applyElementWiseOperator(f1, f2, std::plus<double>());
}

Mapping* Mapping::subtract(ConstMapping &f1, ConstMapping &f2, const Argument& from, const Argument& to)
{
	return applyElementWiseOperator(f1, f2, std::minus<double>());
}
*/


LinearIntplMappingIterator::LinearIntplMappingIterator(ConstMappingIterator* leftIt, ConstMappingIterator* rightIt, double f):
	leftIt(leftIt), rightIt(rightIt), factor(f) {

	assert(leftIt->getPosition() == rightIt->getPosition());
}

LinearIntplMappingIterator::~LinearIntplMappingIterator() {
	if(leftIt)
		delete leftIt;
	if(rightIt)
		delete rightIt;
}
