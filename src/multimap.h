// needchunklist.h
#pragma once

#include "chunk.h"
#include "player.h"
#include <memory>
#include <map>
#include <set>


namespace xim
{
    ////
    // A two-way associative container which allows the representation of a
    // many-to-many relation between two domains of different types.
    //
    template<typename A, typename B>
    class Multimap
    {
    public:
        // Create a relation between two elements
        void add(A a, B b)
        {
            // Store relation in both indexes
            index_a[a].insert(b);
            index_b[b].insert(a);
        }

        // Remove the relation between two elements
        void remove(A a, B b)
        {
            // Remove relation from both indexes, deleting lists if empty
            index_a[a].erase(b);
            if (index_a[a].empty()) {
                index_a.erase(a);
            }

            index_b[b].erase(a);
            if (index_b[b].empty()) {
                index_b.erase(b);
            }
        }

        // Remove all relations for a given element
        void removeAll(A a)
        {
            // Remove all relations (B, a) from the b index
            for (auto it = index_b.begin(); it != index_b.end(); ++it) {
                remove(a, it->first);
            }
        }

        void removeAll(B b)
        {
            // Remove all relations (A, b) from the a index
            for (auto it = index_a.begin(); it != index_a.end(); ++it) {
                remove(it->first, b);
            }
        }


        // Return the number of elements related to a given element
        size_t count(A a) const
        {
            if (index_a.count(a) == 0) {
                return 0;
            }
            return index_a.find(a)->second.size();
        }

        size_t count(B b) const
        {
            if (index_b.count(b) == 0) {
                return 0;
            }
            return index_b.find(b)->second.size();
        }

        // Return the list of elements associated with a given element
        std::set<A> get(B b) const
        {
            if (index_b.count(b) == 0) {
                return std::set<A>();
            }
            return index_b.find(b)->second;
        }

        std::set<B> get(A a) const
        {
            if (index_a.count(a) == 0) {
                return std::set<B>();
            }
            return index_a.find(a)->second;
        }

    private:
        std::map<A, std::set<B>> index_a;
        std::map<B, std::set<A>> index_b;
    };

} // namespace xim

