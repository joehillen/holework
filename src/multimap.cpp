// multimap.cpp
//
#include "multimap.h"

#include <gtest/gtest.h>
#include <iostream>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

// UNIT TESTS

TEST(MultimapTests, Count)
{
    using namespace xim;

    Multimap<std::string, int> map;

    map.add("foo", 1);
    map.add("foo", 2);
    map.add("bar", 1);
    map.add("baz", 5);
    map.add("banana", 1);
    map.add("apple", 1);
    // suppose the programmer really likes pears
    map.add("pear", 50);
    map.add("pear", 50);
    map.add("pear", 50);

    ASSERT_EQ(2, map.count("foo"));
    ASSERT_EQ(1, map.count("bar"));
    ASSERT_EQ(1, map.count("baz"));
    ASSERT_EQ(1, map.count("banana"));
    ASSERT_EQ(1, map.count("apple"));
    ASSERT_EQ(1, map.count("pear"));
    ASSERT_EQ(0, map.count("wibble"));

    ASSERT_EQ(4, map.count(1));
    ASSERT_EQ(1, map.count(5));
    ASSERT_EQ(1, map.count(2));
    ASSERT_EQ(1, map.count(50));
    ASSERT_EQ(0, map.count(9001));
}

template<typename T>
bool operator==(std::set<T> const& a, std::set<T> const& b)
{
    foreach(T element, a) {
        if (b.count(element) == 0) {
            return false;
        }
    }
    return true;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::set<T> set)
{
    os << "set( ";
    foreach(T element, set) {
        os << element << " ";
    }
    os << ")";
    return os;
}

TEST(MultimapTests, Get)
{
    using namespace xim;

    Multimap<std::string, int> map;

    map.add("foo", 100);
    map.add("foo", 101);
    map.add("foo", 102);
    map.add("bar", 100);
    map.add("baz", 100);
    map.add("baz", 101);

    ASSERT_EQ((std::set<std::string>{ "foo", "bar", "baz" }), map.get(100));
    ASSERT_EQ((std::set<int>{ 100, 101, 102 }), map.get("foo"));

    //empty sets
    ASSERT_EQ(std::set<std::string>(), map.get(190));
    ASSERT_EQ(std::set<int>(), map.get("pickle"));
}

TEST(MultimapTests, RemoveAll)
{
    using namespace xim;

    Multimap<std::string, int> map;

    map.removeAll("nonexistent");
    map.removeAll(42152);

    map.add("foo", 100);
    map.add("foo", 101);
    map.add("foo", 102);
    map.add("bar", 100);
    map.add("baz", 100);
    map.add("baz", 101);

    ASSERT_EQ(3, map.count("foo"));
    ASSERT_EQ(3, map.count(100));

    // Removing 100 should decrease foo's relations
    map.removeAll(100);
    ASSERT_EQ(2, map.count("foo"));
    ASSERT_EQ(0, map.count(100));

    // Removing foo should make it go away
    map.removeAll("foo");
    ASSERT_EQ(0, map.count("foo"));

    ASSERT_EQ(1, map.count("baz"));
    map.removeAll("baz");
    ASSERT_EQ(0, map.count("baz"));
}

