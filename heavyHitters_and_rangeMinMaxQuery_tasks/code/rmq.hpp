//
// Created by tam on 2019. 10. 28..
//

#ifndef KHF03_RMQ_HPP
#define KHF03_RMQ_HPP

#include <vector>
#include <iostream>
#include <cmath>

class rmq
{
private:
    std::vector<int> input_v;
    std::vector<std::vector<int>> sparseTable_v;

public:
    rmq(const std::vector<int>&);
    rmq(const rmq&) = default;
    rmq &operator=(const rmq&) = default;
    int max_pos(int left, int right) const;

    std::vector<std::vector<int>> Build_sparseTable(std::vector<int> v) const;
};

rmq::rmq(const std::vector<int> &_v)
{
    input_v = _v;
    sparseTable_v = Build_sparseTable(input_v);

    /*for(size_t i = 0; i < input_v.size(); i++)
    {
        for(size_t j = 0; j < log2(input_v.size())+1; j++)
        {
            std::cout << sparseTable_v[i][j] << ", ";
        }
    }
    std::cout << "\n";*/
}

/*rmq::rmq(const rmq &_range_max_query)
{
    input_v = _range_max_query.input_v;
}*/

/*rmq& rmq::operator=(const rmq &_range_max_query)
{
    rmq &r = const_cast<rmq &>(_range_max_query);

    return r;
}*/

int rmq::max_pos(int left, int right) const
{
    int l = right - left + 1;
    int k = log2(l);

    if(input_v[sparseTable_v[left][k]] >= input_v[sparseTable_v[left+l-(1 << k)][k]])
    {
        return sparseTable_v[left][k];
    }
    else
    {
        return sparseTable_v[left+l-(1 << k)][k];
    }
}

std::vector<std::vector<int>> rmq::Build_sparseTable(std::vector<int> v) const
{
    std::vector<std::vector<int>> st;

    int row = v.size();
    int column = log2(row)+1;

    //std::cout << row << ", " << column << "\n";

    st.resize(row);

    for(size_t i = 0; (int)i < row; i++)
    {
        st[i].resize(column);
    }

    for(size_t i = 0; i < v.size(); i++)
    {
        st[i][0] = i;
    }

    /*for(size_t i = 0; i < v.size(); i++)
    {
        std::cout << v[i] << ", ";
    }
    std::cout << "\n";*/

    for(size_t j = 1; (1 << j) <= (int)v.size(); j++)
    {
        for(size_t i = 0; i+(1 << j)-1 < v.size(); i++)
        {
            if(v[st[i][j-1]] >= v[st[i+(1 << (j-1))][j-1]])
            {
                st[i][j] = st[i][j-1];
            }
            else
            {
                st[i][j] = st[i+(1 << (j-1))][j-1];
            }
        }
    }

    return st;
}

#endif //KHF03_RMQ_HPP
