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
    
    template<class T>
    class Block {
    public:
        size_t sz;
        Node<T> *head, *tail;
        Block *nxt, *pre;
        Block():sz(0), head(nullptr), tail(nullptr), nxt(nullptr), pre(nullptr) {
        }
        Block(T _key):sz(1), nxt(nullptr), pre(nullptr) {
            head = tail = new Node<T>;
            head->key = _key;
        }
        Block(const Block &other): head(nullptr), tail(nullptr) {
            sz = other.sz;
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
        T & at(const size_t &pos) {
            Node<T> *cur = head;
            for (size_t i = 0; i < pos; i++) {
                cur = cur->nxt;
            }
            return cur->key;
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
        private:
            Node<T> *cur;
            Block<T> *blk;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            /**
             * return a new iterator which pointer n-next elements
             *   even if there are not enough elements, the behaviour is **undefined**.
             * as well as operator-
             */
            iterator operator+(const int &n) const {
                
            }
            iterator operator-(const int &n) const {
                //TODO
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                //TODO
            }
            iterator operator+=(const int &n) {
                //TODO
            }
            iterator operator-=(const int &n) {
                //TODO
            }
            /**
             * TODO iter++
             */
            iterator operator++(int) {}
            /**
             * TODO ++iter
             */
            iterator& operator++() {}
            /**
             * TODO iter--
             */
            iterator operator--(int) {}
            /**
             * TODO --iter
             */
            iterator& operator--() {}
            /**
             * TODO *it
             */
            T& operator*() const {}
            /**
             * TODO it->field
             */
            T* operator->() const noexcept {}
            /**
             * a operator to check whether two iterators are same (pointing to the same memory).
             */
            bool operator==(const iterator &rhs) const {}
            bool operator==(const const_iterator &rhs) const {}
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {}
            bool operator!=(const const_iterator &rhs) const {}
        };
        class const_iterator {
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
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
        };
        /**
         * TODO Constructors
         */
        deque():sz(2) {
            head = tail = new Block<T>;
        }
        deque(const deque &other) {
            sz = other.sz;
            head = new Block<T>(*(other.head));
            Block<T> *ptr = head;
            Block<T> *cur = other.head;
            while(cur->nxt != nullptr) {
                cur = cur->nxt;
                ptr->nxt = new Block<T>(*cur);
                ptr->nxt->pre = ptr;
            }
        }
        /**
         * TODO Deconstructor
         */
        ~deque() {
            Block<T> *cur = head, *tmp;
            while(cur != nullptr) {
                tmp = cur;
                cur = cur->nxt;
                delete tmp;
            }
        }
        /**
         * TODO assignment operator
         */
        deque &operator=(const deque &other) {
            sz = other.sz;
            head = new Block<T>(*(other.head));
            Block<T> *ptr = head;
            Block<T> *cur = other.head;
            while(cur->nxt != nullptr) {
                cur = cur->nxt;
                ptr->nxt = new Block<T>(*cur);
                ptr->nxt->pre = ptr;
            }
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
            while (tmp > cur->sz) {
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
            while (tmp > cur->sz) {
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
        iterator begin() {}
        const_iterator cbegin() const {}
        /**
         * returns an iterator to the end.
         */
        iterator end() {}
        const_iterator cend() const {}
        /**
         * checks whether the container is empty.
         */
        bool empty() const {}
        /**
         * returns the number of elements
         */
        size_t size() const {}
        /**
         * clears the contents
         */
        void clear() {}
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {}
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {}
        /**
         * adds an element to the end
         */
        void push_back(const T &value) {}
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
