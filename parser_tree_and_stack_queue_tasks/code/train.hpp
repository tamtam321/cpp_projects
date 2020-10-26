//
// Created by tam on 2019. 10. 05..
//

#ifndef KHF02_1_03_TRAIN_HPP
#define KHF02_1_03_TRAIN_HPP

#include <iostream>
#include <stack>
#include <queue>
#include <algorithm>

bool is_orderable(const std::vector<short> &cars, bool stack, bool queue)
{
    std::stack<short> s;
    std::queue<short> q;
    std::vector<short> _out;
    short _min = *std::min_element(cars.begin(), cars.end());
    unsigned i = 0;

    while(_out.size() != cars.size())
    {
        if(stack)
        {
            while(s.size() != 0 && i <= (cars.size()-1))
            {
                if(_min >= s.top())
                {
                    _out.push_back(s.top());
                    s.pop();
                }
                else
                    break;
            }

            while(s.size() != 0 && i >= (cars.size()))
            {
                //if(_min <= s.top())
                _out.push_back(s.top());

                s.pop();
            }

            if(i <= (cars.size()-1))
            {
                if(_min == cars[i])
                {
                    _out.push_back(cars[i]);
                }
                else if(_min < cars[i])
                {
                    if(s.size() != 0 && s.top() < cars[i])
                    {
                        return false;
                    }
                    else
                    {
                        s.push(cars[i]);
                    }
                }
            }
        }
        else if(queue)
        {
            while(q.size() != 0 && i <= (cars.size()-1))
            {
                if(_min >= q.front())
                {
                    _out.push_back(q.front());
                    q.pop();
                }
                else
                    break;
            }

            while(q.size() != 0 && i >= (cars.size()))
            {
                if(_min <= q.front())
                    _out.push_back(q.front());

                q.pop();
            }

            if(i <= (cars.size()-1))
            {
                if(_min == cars[i])
                {
                    _out.push_back(cars[i]);
                }
                else if(_min < cars[i])
                {
                    if(q.size() != 0 && q.back() > cars[i])
                    {
                        return false;
                    }
                    else
                    {
                        q.push(cars[i]);
                    }
                }
            }
        }

        ++i;

        if(i <= (cars.size()-1))
            _min = *std::min_element(cars.begin()+i, cars.end());
    }

    return true;
}

#endif //KHF02_1_03_TRAIN_HPP
