#pragma once
#include <iterator>
#include <functional>
#include <cstdlib>



namespace LuaSTGPlus::Particle
{
    template<typename P>
    class ParticleList
    {
    private:
        //static constexpr int32_t TAIL_NODE = -1;
        //static constexpr int32_t FREE_NODE = -2;
        static constexpr int32_t INVALID_INDEX = -1;
    public:
        void insert(P val) {
            //for (int32_t i = 0; i < size; i++)
            //{
            //    if (arr[i].next == FREE_NODE)
            //    {
            //        arr[i] = Node(std::move(val));
            //        arr[tail].next = i;
            //        tail = i;
            //        if (head == FREE_NODE)
            //            head = tail;

            //        return;
            //    }
            //}
            // It's full. Take the head and make it the tail.
            //int32_t temp = arr[head].next;
            //arr[head] = Node(std::move(val));
            //arr[tail].next = head;
            //tail = head;
            //head = temp;
            if (free == INVALID_INDEX)
            {
                if (maxn >= size)
                {
                    int32_t temp = arr[back].next;
                    arr[back] = Node(std::move(val));
                    if (front != INVALID_INDEX)
                        arr[front].next = back;
                    front = back;
                    back = temp;
                }
                else
                {
                    arr[maxn] = Node(std::move(val));
                    //arr[maxn].next = front;
                    if (front != INVALID_INDEX)
                        arr[front].next = maxn;
                    front = maxn;
                    if (back == INVALID_INDEX)
                        back = front;
                    maxn++;
                }
            }
            else
            {
                int32_t temp = arr[free].next;
                arr[free] = Node(std::move(val));
                if (front != INVALID_INDEX)
                    arr[front].next = free;
                front = free;
                if (back == INVALID_INDEX)
                    back = front;
                free = temp;
            }
        }
    private:
        //class ForeachContext;
    public:
        // Iterate over each particle, in a way that guarantees efficient removal.
        //void foreach_with_ctx(std::function<void(P* const, ForeachContext)> fn)
        void foreach(std::function<bool(P* const)> fn)
        {
            //if (head == FREE_NODE)
            if (back == INVALID_INDEX)
                return;


            ////auto ctx = ForeachContext(head, *this);
            //auto ctx = ForeachContext(back, *this);
            ////while (ctx.current != TAIL_NODE)
            ////{
            ////    int32_t next = arr[ctx.current].next;
            ////    fn(&*arr[ctx.current], ctx);
            ////    if (arr[ctx.current].next != FREE_NODE)
            ////        ctx.last = ctx.current;
            ////    ctx.current = next;
            ////}
            //while (ctx.current != INVALID_INDEX)
            //{
            //    int32_t next = arr[ctx.current].next;
            //    fn(&*arr[ctx.current], ctx);
            //    if (arr[ctx.current].next != INVALID_INDEX)
            //        ctx.last = ctx.current;
            //    ctx.current = next;
            //}
            int32_t current = back;
            int32_t last = INVALID_INDEX;
            while (current != INVALID_INDEX)
            {
                int32_t temp = arr[current].next;
                if (fn(&arr[current].val))
                {
                    if (current == back)
                    {
                        if (back == front)
                            front = arr[current].next;
                        back = arr[current].next;
                    }
                    else
                        arr[last].next = arr[current].next;

                    arr[current].next = free;
                    free = current;
                }

                last = current;
                current = temp;
                //current = next;
            }
        }
    public:
        ParticleList(int32_t size) : arr(new Node[size]), size(size) {
            //spdlog::debug("[particle] sizeof Node: {}", sizeof(Node));
            //spdlog::debug("[particle] Address of arr: {}", fmt::ptr(arr));
        }
        ~ParticleList()
        {
            delete [] arr;
        }
    public:
        int32_t GetSize() { return size; }
        //P* GetTail() { return &arr[tail].val; }
        P* GetFront() { return &arr[front].val; }
    private:
        //class ForeachContext
        //{
        //public:
        //    void delete_current()
        //    {
        //        //if (current != pl.head)
        //        //    pl.arr[last].next = pl.arr[current].next;

        //        //pl.arr[current].next = FREE_NODE;
        //        if (current != pl.back)
        //            pl.arr[last].next = pl.arr[current].next;
        //        else
        //        {
        //            pl.back = pl.arr[current].next;
        //        }

        //        pl.arr[current].next = pl.free;
        //        pl.free = current;
        //    }
        //private:
        //    ForeachContext(int32_t idx, ParticleList& pl) : current(idx), pl(pl) {}
        //    //int32_t current = FREE_NODE;
        //    //int32_t last = FREE_NODE;
        //    int32_t current = INVALID_INDEX;
        //    int32_t last = INVALID_INDEX;
        //    ParticleList& pl;
        //    friend class ParticleList;
        //};
        //friend class ForeachContext;
    public:
        struct Iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using diffrence_type = std::ptrdiff_t;
            using value_type = P;
            using pointer = P*;
            using reference = P&;
        public:
            Iterator(int32_t idx, ParticleList* pl) : node_idx(idx), pl(pl) {}
            reference operator*() const { return *pl->arr[node_idx]; }
            pointer operator->() { return pl->arr[node_idx]; }
            Iterator& operator++() { node_idx = pl->arr[node_idx].next; return *this; }
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.node_idx == b.node_idx; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.node_idx != b.node_idx; };
        private:
            int32_t node_idx;
            ParticleList* pl;
        };
        friend class Iterator;

        //Iterator begin() { return Iterator(head != FREE_NODE ? head : TAIL_NODE, this); }
        //Iterator end() { return Iterator(TAIL_NODE, this); }
        Iterator begin() { return Iterator(back, this); }
        Iterator end() { return Iterator(INVALID_INDEX, this); }
    private:
        class Node {
        public:
            //Node() : val(), next(FREE_NODE) {}
            //Node(P v) : val(v), next(TAIL_NODE) {}
            Node() : val(), next(INVALID_INDEX) {}
            Node(P v) : val(v), next(INVALID_INDEX) {}
            //void operator delete[](void* n) { ::free(n); }
            P& operator*() { return val; }
            P* operator->() { return &val; }
            P* operator&() { return &val; }
        private:
            P val;
            int32_t next;
        private:
            friend class ParticleList;
            friend class Iterator;
        };
    private:
        Node* arr = nullptr;
        int32_t size;
        //int32_t head = FREE_NODE;
        //int32_t tail = FREE_NODE;
        int32_t maxn = 0;
        int32_t front = INVALID_INDEX;
        int32_t back = INVALID_INDEX;
        int32_t free = INVALID_INDEX;
    };
}
