#pragma once

#include "ptr.h"
#include <set>
#include <map>

// Generic container for handling OwnPtr and Ptr objects. It's needed because std::map/set<OwnPtr>::find can't accept Ptrs, even though a less operator is called.
// This may be fixed in C++14
template <class T, class Compare = std::less<OwnPtr<T>>> class PtrSet
{
public:
	typename std::set<OwnPtr<T>, Compare>::iterator insert(OwnPtr<T> object);

	typename std::set<OwnPtr<T>, Compare>::iterator erase(Ptr<T> object);

	bool empty() const;

	typename std::set<OwnPtr<T>, Compare>::size_type size() const;

	typename std::set<OwnPtr<T>, Compare>::iterator find(Ptr<T> object);

	typename std::set<OwnPtr<T>, Compare>::const_iterator find(Ptr<T> object) const;

	typename std::set<OwnPtr<T>, Compare>::iterator begin();

	typename std::set<OwnPtr<T>, Compare>::iterator end();

	typename std::set<OwnPtr<T>, Compare>::const_iterator cbegin() const;

	typename std::set<OwnPtr<T>, Compare>::const_iterator cend() const;

private:
	std::set<OwnPtr<T>, Compare> objects;
	std::map<Ptr<T>, typename std::set<OwnPtr<T>, Compare>::iterator> objectLookup;
};

// Template Implementation

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::iterator PtrSet<T, Compare>::insert(OwnPtr<T> object)
{
	auto iterator = objects.insert(object);
	objectLookup[object] = iterator.first;
	return iterator.first;
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::iterator PtrSet<T, Compare>::erase(Ptr<T> object)
{
	auto iterator = objectLookup.find(object);
	typename std::set<OwnPtr<T>, Compare>::iterator returnIterator;
	if(iterator != objectLookup.end())
	{
		returnIterator = objects.erase(iterator->second);
		objectLookup.erase(iterator);
	}
	else
	{
		returnIterator = objects.end();
	}
	return returnIterator;
}

template <class T, class Compare>
bool PtrSet<T, Compare>::empty() const
{
	return objects.empty();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::size_type PtrSet<T, Compare>::size() const
{
	return objects.size();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::iterator PtrSet<T, Compare>::find(Ptr<T> object)
{
	auto iterator = objectLookup.find(object);
	if(iterator != objectLookup.end())
	{
		return iterator->second;
	}
	return objects.end();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::const_iterator PtrSet<T, Compare>::find(Ptr<T> object) const
{
	auto iterator = objectLookup.find(object);
	if(iterator != objectLookup.end())
	{
		return iterator->second;
	}
	return objects.end();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::iterator PtrSet<T, Compare>::begin()
{
	return objects.begin();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::iterator PtrSet<T, Compare>::end()
{
	return objects.end();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::const_iterator PtrSet<T, Compare>::cbegin() const
{
	return objects.cbegin();
}

template <class T, class Compare>
typename std::set<OwnPtr<T>, Compare>::const_iterator PtrSet<T, Compare>::cend() const
{
	return objects.cend();
}

