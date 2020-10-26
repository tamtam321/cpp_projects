//
// Created by tam on 2019. 10. 26..
//

#ifndef KHF03_HEAVY_HITTERS_HPP
#define KHF03_HEAVY_HITTERS_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>

template <typename ForwardIterator, typename ValueType = typename std::iterator_traits<ForwardIterator>::value_type>
std::vector<ValueType> heavy_hitters(ForwardIterator begin, ForwardIterator end, size_t k)
{
    std::unordered_map<ValueType, size_t> possible_majority;
    std::vector<ValueType> sol;

    bool flag = false;
    size_t _size = end - begin;

    for(ForwardIterator i = begin; i != end; i++)
    {
        if(possible_majority.size() < k-1)
        {
            if(!possible_majority.empty())
            {
                flag = false;

                if(possible_majority.count(*i) > 0)
                {
                    ++possible_majority.find(*i)->second;
                    flag = true;
                }
            }

            if(!flag)
            {
                possible_majority.insert(std::pair<ValueType, size_t>(*i, 1));
            }
        }
        else  // possible_majority.size() == k-1
        {
            flag = false;

            if(possible_majority.count(*i) > 0)
            {
                ++possible_majority.find(*i)->second;
                flag = true;
            }

            if(!flag)
            {
                /*for(typename std::unordered_map<ValueType, size_t>::iterator it = possible_majority.begin();
                    it != possible_majority.end(); it++)
                {
                    if(it->second > 1)
                    {
                        --it->second;
                    }
                }*/

                for(typename std::unordered_map<ValueType, size_t>::iterator it = possible_majority.begin();
                    it != possible_majority.end();)
                {
                    if (it->second == 1)
                    {
                        possible_majority.erase(it++);
                    }
                    else
                    {
                        --it->second;
                        ++it;
                    }
                }
            }
        }
    }

    for(typename std::unordered_map<ValueType, size_t>::iterator it = possible_majority.begin();
        it != possible_majority.end(); it++)
    {
        it->second = 0;
    }

    for(ForwardIterator i = begin; i != end; i++)
    {
        if(possible_majority.count(*i) > 0)
        {
            ++possible_majority.find(*i)->second;
        }
    }

    for(typename std::unordered_map<ValueType, size_t>::iterator it = possible_majority.begin();
        it != possible_majority.end(); it++)
    {
        if(it->second > _size/k)
        {
            sol.push_back(it->first);
        }
    }

    return sol;
}

#endif //KHF03_HEAVY_HITTERS_HPP
