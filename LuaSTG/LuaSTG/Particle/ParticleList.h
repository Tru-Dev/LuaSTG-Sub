#pragma once
#include <iterator>
#include <functional>

namespace LuaSTGPlus::Particle
{
    template<typename P>
    class ParticleList
    {
    public:
        void insert(P val) {
            for (int32_t i = 0; i < size; i++)
            {
                if (arr[i].next == FREE_NODE)
                {
                    arr[i] = Node(val);
                    arr[tail].next = i;
                    tail = i;
                    if (head == FREE_NODE)
                        head = tail;

                    return;
                }
            }
            // It's full. Take the head and make it the tail.
            int32_t temp = arr[head].next;
            arr[head] = Node(val);
            arr[tail].next = head;
            tail = head;
            head = temp;
        }
    private:
        class ForeachContext;
    public:
        // Iterate over each particle, in a way that guarantees efficient removal.
        void foreach_with_ctx(std::function<void(const P&, ForeachContext)> fn)
        {
            if (head == FREE_NODE)
                return;

            auto ctx = ForeachContext();
            while (ctx.current != TAIL_NODE)
            {
                fn(arr[ctx.current], ctx);
                ctx.last = ctx.current;
                ctx.current = arr[ctx.current].next;
            }
        }
    public:
        ParticleList(int32_t size) : arr(new Node[size]), size(size) {}

        ~ParticleList() { delete[] arr; }
    private:
        class ForeachContext
        {
        public:
            void delete_current()
            {
                if (current != pl.  head)
                    pl.arr[last].next = pl.arr[current].next;

                pl.arr[current].next = FREE_NODE;
            }
        private:
            ForeachContext(int32_t idx, ParticleList& pl) : current(idx), pl(pl) {}
            int32_t current = FREE_NODE;
            int32_t last = FREE_NODE;
            ParticleList& pl;
            friend class ParticleList;
        };
        friend class ForeachContext;
    public:
        struct Iterator
        {
            using iterator_category = std::forward_iterator_tag;
            using diffrence_type = std::ptrdiff_t;
            using value_type = P;
            using pointer = P*;
            using reference = P&;

            Iterator(int32_t idx, ParticleList& pl) : node_idx(idx), pl(pl) {}
            reference operator*() const { return *pl.arr[node_idx]; }
            pointer operator->() { return pl.arr[node_idx]; }
            Iterator& operator++() { node_idx = pl.arr[node_idx].next; return *this; }
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.node_idx == b.node_idx; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.node_idx != b.node_idx; };
        private:
            int32_t node_idx;
            ParticleList& pl;
        };
        friend class Iterator;

        Iterator begin() { return Iterator(head != FREE_NODE ? head : TAIL_NODE); }
        Iterator end() { return Iterator(TAIL_NODE); }
    private:
        static constexpr int32_t TAIL_NODE = -1;
        static constexpr int32_t FREE_NODE = -2;
    private:
        class Node {
        public:
            Node(P v) : val(v), next(TAIL_NODE) {}
            P& operator*() const { return val; }
            P operator->() const { return &val; }
        private:
            P val;
            // Negative values are taken to mean an invalid node.
            int32_t next = FREE_NODE;
        private:
            friend class ParticleList;
            friend class Iterator;
        };
    private:
        Node* arr;
        int32_t size;
        int32_t head = FREE_NODE;
        int32_t tail = FREE_NODE;
    };
}

