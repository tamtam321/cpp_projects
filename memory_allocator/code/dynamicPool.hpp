#pragma once
#include "fixedSizePool.hpp"
#include <memory>

template<class T, unsigned N>
class DynamicPool
{
private:
    class Node
    {
    public:
        Node *pNext, *pPrev;
        FixedSizePool<T, N> fsp;

        Node() : pNext(nullptr), pPrev(nullptr) {}
        Node(Node *prev) : pNext(nullptr), pPrev(prev) {}
    };

    Node *head, *tail;

public: //A return nullptr es a (void)variable csak a warning-ok kiiktatasa miatt vannak ott,
        //torold oket, amikor elkezded az adott fuggvenyt implementalni 
    DynamicPool() : head(nullptr) {};

    T* allocate()
    {
        if(head == nullptr)
        {
            Node *n = new Node();
            head = tail = n;
            return n->fsp.allocate();
        }
        else
        {
            for(Node *i = head; i != nullptr; i = i->pNext)
            {
                if(!i->fsp.get_isFull())
                {
                    return i->fsp.allocate();
                }
                else
                {
                    Node *n = new Node(tail);
                    tail->pNext = n;
                    tail = n;
                    return n->fsp.allocate();
                }
            }
        }

        /*if(head->fsp.get_isFull())
        {
            Node *n = new Node(tail);
            tail->pNext = n;
            tail = n;
            return n->fsp.allocate();
        }
        else
        {
            return tail->fsp.allocate();
        }*/
    }

    T* allocate(unsigned n)
    {
        (void)n;
        return nullptr;
    }

    void deallocate(T* p)
    {
        (void)p;
    }

    void deallocate(T* p, unsigned n)
    {
        (void)n;
        (void)p;
    }

    T* construct(const T value)
    {
        (void)value;
        return nullptr;
    }
};
