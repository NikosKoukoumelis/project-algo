#ifndef __TABLE_ITEM_TYPES_H__
#define __TABLE_ITEM_TYPES_H__

#include <vector>

struct HashTableItem                            //container for item used at Hash table(-contains hashid)
{
    int itemId;
    long hashId;
    std::vector<float> v;

    bool operator==(HashTableItem i)
    {
        return v == i.v && hashId == i.hashId;
    }
};

struct HypercubeItem                            //container for item used at Hypercube (and some other thinf, contains itemid and the vector of the item)
{
    int itemId;
    std::vector<float> v;

    bool operator==(HypercubeItem i)
    {
        return v == i.v;
    }
};

#endif
