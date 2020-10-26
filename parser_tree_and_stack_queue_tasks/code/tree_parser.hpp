//
// Created by tam on 2019. 10. 05..
//

#ifndef KHF02_1_03_TREE_PARSER_HPP
#define KHF02_1_03_TREE_PARSER_HPP

#include <exception>
#include <iostream>
#include <stack>
#include <string>
#include <stdlib.h>
#include <sstream>

///A feladatban megadott kivétel származzon a std::exception osztályból:
struct TreeException : public std::exception
{
    const char *what() const throw() { return "The given path was too long or too short !"; }
};

class TernaryTree
{
private:
    std::string _input;

    struct Node
    {
        int _data;
        Node *Left, *Middle, *Right, *Parent;

        /// Sima Node. Nincs semmije
        Node() : Left(nullptr), Middle(nullptr), Right(nullptr), Parent(nullptr) {}
    };

    Node *root;

public:
    /// stringstream: char to int
    int _charToint(char ch)
    {
        int i;
        std::stringstream ss;
        ss << ch;
        ss >> i;

        return i;
    }

    /// Destruktor ezt hívja meg
    void _destroy(Node* x)
    {
        if(x != nullptr)
        {
            _destroy(x->Left);
            _destroy(x->Middle);
            _destroy(x->Right);

            delete x;
        }
    }

    TernaryTree() : root(nullptr) {};

    /// Destruktor
    ~TernaryTree()
    {
        _destroy(root);
    }

    TernaryTree(const std::string &input) : _input(input)
    {
        /// Aktuális pzíció
        Node *act = nullptr;

        for(unsigned i = 0; i < _input.length(); i++)
        {
            if(_input[i] == '(')
            {
                if(act == nullptr)
                {
                    act = new Node();
                    root = act;
                }
                else if(act->Left == nullptr)
                {
                    act->Left = new Node();
                    act->Left->Parent = act;
                    act = act->Left;
                }
                else if(act->Middle == nullptr)
                {
                    act->Middle = new Node();
                    act->Middle->Parent = act;
                    act = act->Middle;
                }
                else
                {
                    act->Right = new Node();
                    act->Right->Parent = act;
                    act = act->Right;
                }
            }
            else if(std::isdigit(_input[i]))
            {
                if(act->Left == nullptr)
                {
                    act->Left = new Node();
                    act->Left->_data = _charToint(_input[i]);
                    act->Left->Parent = act;
                }
                else if(act->Middle == nullptr)
                {
                    act->Middle = new Node();
                    act->Middle->_data = _charToint(_input[i]);
                    act->Middle->Parent = act;
                }
                else
                {
                    act->Right = new Node();
                    act->Right->_data = _charToint(_input[i]);
                    act->Right->Parent = act;
                }
            }
            else if(_input[i] == ')')
            {
                if(act->Parent != nullptr)
                    act = act->Parent;
            }
        }
    }

    int get_value(const std::vector<short> &path) const
    {
        Node *act = root;
        TreeException te;

        for(unsigned i = 0; i < path.size(); i++)
        {
            if(path[i] == 0)
            {
                if(act->Left != nullptr)
                    act = act->Left;
                else
                    throw te;
            }
            else if(path[i] == 1)
            {
                if(act->Middle != nullptr)
                    act = act->Middle;
                else
                    throw te;
            }
            else if(path[i] == 2)
            {
                if(act->Right != nullptr)
                    act = act->Right;
                else
                    throw te;
            }
        }

        if(act->Left != nullptr)
            throw te;
        else
            return act->_data;
    }
};

#endif //KHF02_1_03_TREE_PARSER_HPP
