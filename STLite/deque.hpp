#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

    template<class T>
    class Node {
    public:
        T key;
        Node *nxt, *pre;
        Node():nxt(nullptr), pre(nullptr) {
        }
        Node(T _key):nxt(nullptr), pre(nullptr) {
            key = _key;
        }
        void set(T _key, Node<T>* _nxt, Node<T>* _pre) {
            key = _key;
            nxt = _nxt;
            pre = _pre;
        }
    };

    template<class T> class deque;
    
    template<class T>
    class Block {
    public:
        size_t sz;
        Node<T> *head, *tail;
        Block *nxt, *pre;
        deque<T> *que;
        Block():sz(0), head(nullptr), tail(nullptr), nxt(nullptr), pre(nullptr) {
        }
        Block(T _key, deque<T> *_que):sz(1), nxt(nullptr), pre(nullptr) {
            head = tail = new Node<T>;
            head->key = _key;
            que = _que;
        }
        Block(const Block &other): head(nullptr), tail(nullptr) {
            sz = other.sz;
            que = other.que;
            if (other.sz == 0) {
                return;
            }
            head = tail = new Node<T>;
            head->key = other.head->key;
            Node<T> *ptr = head;
            Node<T> *cur = other.head;
            while(cur->nxt != nullptr) {
                cur = cur->nxt;
                ptr->nxt = new Node<T>;
                ptr->nxt->key = cur->key;
                ptr->nxt->pre = ptr;
            }
        }
        ~Block() {
            printf("~~~~~\n");
            Node<T> *cur = head, *tmp;
            while(cur != nullptr) {
                tmp = cur;
                cur = cur->nxt;
                delete tmp;
            }
        }
        void insertAt(size_t idx, T _key) {
            sz = sz + 1;
            if (idx == 0) {
                Node<T> *tmp = head;
                head = new Node<T>;
                head->set(_key, tmp, nullptr);
            } else {
                Node<T> *cur = head, *tmp;
                while (idx--) {
                    cur = cur->nxt;
                }
                tmp = cur->nxt;
                cur->nxt = new Node<T>;
                cur->nxt->set(_key, tmp, cur);
            }
        }
        void deleteAt(size_t idx) {
            sz = sz - 1;
            Node<T> *tmp = head;
            while (idx--) {
                tmp = tmp->nxt;
            }
            if (tmp->pre != nullptr) {
                tmp->pre->nxt = tmp->nxt;
            }
            if (tmp->nxt != nullptr) {
                tmp->nxt->pre = tmp->pre;
            }
            delete tmp;
        }
        void pushBack(T _key) {
            sz = sz + 1;
            tail->nxt = new Node<T>;
            tail->nxt->set(_key, nullptr, tail);
            tail = tail->nxt;
        }
        void pushFront(T _key) {
            sz = sz + 1;
            head->pre = new Node<T>;
            head->pre->set(_key, head, nullptr);
            head = head->pre;
        }
        T & at(const size_t &pos) {
            Node<T> *cur = head;
            for (size_t i = 0; i < pos; i++) {
                cur = cur->nxt;
            }
            return cur->key;
        }
        void popFront() {
            sz = sz - 1;
            Node<T> *tmp = head;
            head = head->nxt;
            head->pre = nullptr;
            delete tmp;
        }
        void popBack() {
            sz = sz - 1;
            Node<T> *tmp = tail;
            tail = tail->pre;
            tail->nxt = nullptr;
            delete tmp;
        }
        const T & at(const size_t &pos) const {
            Node<T> *cur = head;
            for (size_t i = 0; i < pos; i++) {
                cur = cur->nxt;
            }
            return cur->key;
        }
        void print() {
            auto cur = head;
            while (cur != nullptr) {
                printf("%d ", cur->key);
                cur = cur->nxt;
            }
            printf("\n");
        }
    };

    template<class T>
    class deque {
    private:
        size_t sz;
        Block<T> *head, *tail;

    public:
        class const_iterator;

        class iterator {
            
            friend iterator deque::insert(iterator pos, const T &value);
            friend iterator deque::erase(iterator pos);
            
        private:
            Node<T> *cur;
            Block<T> *blk;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
            size_t getIndexToBlockFront() {
                size_t ret = 0;
                Node<T> *tmp = cur;
                while (tmp != nullptr) {
                    ret = ret + 1;
                    tmp = tmp->pre;
                }
                return ret;
            }
            size_t getIndexToBlockBack() {
                size_t ret = 0;
                Node<T> *tmp = cur;
                while (tmp != nullptr) {
                    ret = ret + 1;
                    tmp = tmp->nxt;
                }
                return ret - 1;
            }
            size_t getIndexToDequeFront() {
                size_t ret = getIndexToBlockFront();
                Block<T> *tmp = blk->pre;
                while (tmp != nullptr) {
                    ret = ret + tmp->sz;
                    tmp = tmp->pre;
                }
                return ret;
            }
            void selfPlus(const int &n) {
                int pos = n;
                int idx = getIndexToBlockBack();
                if (idx >= pos) {
                    while (pos--) {
                        cur = cur->nxt;
                    }
                } else {
                    pos -= idx;
                    blk = blk->nxt;
                    while (blk != nullptr && pos > blk->sz) {
                        pos -= blk->sz;
                        blk = blk->nxt;
                    }
                    cur = blk->head;
                    while (pos--) {
                        cur = cur->nxt;
                    }
                }
            }
            void selfMinus(const int &n) {
                int pos = n;
                int idx = getIndexToBlockFront();
                if (idx >= pos) {
                    while (pos--) {
                        cur = cur->pre;
                    }
                } else {
                    pos -= idx;
                    blk = blk->pre;
                    while (blk != nullptr && pos > blk->sz) {
                        pos -= blk->sz;
                        blk = blk->pre;
                    }
                    cur = blk->tail;
                    while (pos--) {
                        cur = cur->pre;
                    }
                }
            }
            
        public:
            iterator() {
                
            }
            iterator(Node<T> *_cur, Block<T> *_blk) {
                cur = _cur;
                blk = _blk;
            }
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                iterator ret = *this;
                ret.selfPlus(n);
                return ret;
            }
            iterator operator-(const int &n) const {
                iterator ret = *this;
                ret.selfMinus(n);
                return ret;
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (blk->que != rhs.blk->que) {
                    throw invalid_iterator();
                }
                return getIndexToDequeFront() - rhs.getIndexToDequeFront();
            }
            iterator operator+=(const int &n) {
                selfPlus(n);
                return *this;
            }
            iterator operator-=(const int &n) {
                selfMinus(n);
                return *this;
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {
                iterator ret = *this;
                selfPlus(1);
                return ret;
            }
            /**
             * TODO ++iter
             */
            iterator& operator++() {
                selfPlus(1);
                return *this;
            }
            /**
             * TODO iter--
             */
            iterator operator--(int) {
                iterator ret = *this;
                selfMinus(1);
                return ret;
            }
            /**
             * TODO --iter
             */
            iterator& operator--() {
                selfMinus(1);
                return *this;
            }
            /**
             * TODO *it
             */
            T& operator*() const {
                return cur->key;
            }
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {
                return &(cur->key);
            }
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {
                return blk == rhs.blk && cur == rhs.cur;
            }
            bool operator==(const const_iterator &rhs) const {
                return blk == rhs.blk && cur == rhs.cur;
            }
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(*this == rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this == rhs);
            }
        };
        
        class const_iterator : public iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            // data members.
        public:
            const_iterator() {
                // TODO
            }
            const_iterator(const const_iterator &other) {
                // TODO
            }
            const_iterator(const iterator &other) {
                // TODO
            }
            T& operator*() const {
                T ret = T();
                return ret;
            }
        };
        
    private:
        void copyFrom(const deque &other) {
            sz = other.sz;
            head = new Block<T>(*(other.head));
            Block<T> *ptr = head;
            Block<T> *cur = other.head;
            while(cur->nxt != nullptr) {
                cur = cur->nxt;
                ptr->nxt = new Block<T>(*cur, this);
                ptr->nxt->pre = ptr;
            }
        }
        void deconstruct() {
            Block<T> *cur = head, *tmp;
            while(cur != nullptr) {
                tmp = cur;
                cur = cur->nxt;
                delete tmp;
            }
        }
    public:
        /**
         * TODO Constructors
         */
        deque():sz(0) {
            head = tail = new Block<T>;
        }
        deque(const deque &other) {
            copyFrom(other);
        }
        /**
         * TODO Deconstructor
         */
        ~deque() {
            deconstruct();
        }
        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            copyFrom(other);
            return *this;
        }
        /**
         * access specified element with bounds checking
         * throw index_out_of_bound if out of bound.
         */
        T & at(const size_t &pos) {
            size_t tmp = pos;
            if (tmp > sz) {
                throw index_out_of_bound();
            }
            Block<T> *cur = head;
            while (tmp >= cur->sz) {
                tmp -= cur->sz;
                cur = cur->nxt;
            }
            return cur->at(tmp);
        }
        const T & at(const size_t &pos) const {
            size_t tmp = pos;
            if (tmp > sz) {
                throw index_out_of_bound();
            }
            Block<T> *cur = head;
            while (tmp >= cur->sz) {
                tmp -= cur->sz;
                cur = cur->nxt;
            }
            return cur->at(tmp);
        }
        T & operator[](const size_t &pos) {
            return at(pos);
        }
        const T & operator[](const size_t &pos) const {
            return at(pos);
        }
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T & front() const {
            if (sz == 0) {
                throw container_is_empty();
            }
            return head->head->key;
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            if (sz == 0) {
                throw container_is_empty();
            }
            return tail->tail->key;
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {
            return iterator(head->head, head, 0, 0);
        }
        const_iterator cbegin() const {
            
        }
        /**
         * returns an iterator to the end.
         */
        iterator end() {
            return iterator(tail->tail, tail, tail->sz - 1, sz - 1);
        }
        const_iterator cend() const {
            
        }
        /**
         * checks whether the container is empty.
         */
        bool empty() const {
            return sz == 0;
        }
        /**
         * returns the number of elements
         */
        size_t size() const {
            return sz;
        }
        /**
         * clears the contents
         */
        void clear() {
            deconstruct();
            sz = 0;
            head = tail = new Block<T>;
        }
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            Node<T> *nd = new Node<T>;
            nd->key = value;
            nd->pre = pos->cur->pre;
            nd->nxt = pos->cur;
            pos->cur->pre = nd;
            pos->cur->pre->nxt = nd;
            pos.cur = nd;
            return *this;
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            Node<T> *pr = pos.cur->pre;
            Node<T> *nx = pos.cur->nxt;
            pr->nxt = nx;
            nx->pre = pr;
        }
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {
            
        }
        /**
         * removes the last element
         *     throw when the container is empty.
         */
        void pop_back() {}
        /**
         * inserts an element to the beginning.
         */
        void push_front(const T &value) {}
        /**
         * removes the first element.
         *     throw when the container is empty.
         */
        void pop_front() {}
    };

}

#endif
