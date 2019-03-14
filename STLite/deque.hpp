#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <iostream>

namespace sjtu {

    const size_t bSize = 256;
    
    template<class T>
    class deque {
    public:
        class const_iterator;
        class Block;
    // private:
        Block *head, *tail;
        size_t sz;
        void copyFrom(const deque &other) {
            sz = other.sz;
            head = new Block(this);
            for (int i = 0; i < other.head->cnt; i++) {
                head->pushBack(*(other.head->at(i)));
            }
            Block *ptr = head;
            for (Block *cur = other.head->nxt; cur != nullptr; cur = cur->nxt, ptr = ptr->nxt) {
                ptr->nxt = new Block(this);
                ptr->nxt->pre = ptr;
                for (int i = 0; i < cur->cnt; i++) {
                    ptr->nxt->pushBack(*(cur->at(i)));
                }
            }
            tail = ptr;
        }
    public:
        class iterator {
        public:
            friend iterator deque::insert(iterator pos, const T &value);
            friend iterator deque::erase(iterator pos);
//        private:
            Block *blk;
            int idx;
            void plus(int n) {
//                printf("...%d %d %d\n", blk->cnt, idx, n);
                if (blk->cnt - idx > n || (blk == blk->deq->tail && blk->cnt - idx == n)) {
                    idx += n;
                } else {
                    n -= (blk->cnt - idx);
                    for (blk = blk->nxt; ; n -= blk->cnt, blk = blk->nxt) {
                        if (n < blk->cnt) {
                            idx = n;
                            break;
                        }
                    }
                }
            }
            void minus(int n) {
//                printf("___%d %d %d\n", blk->cnt, idx, n);
                if (idx >= n) {
                    idx -= n;
                } else {
                    n -= (idx + 1);
                    for (blk = blk->pre; ; n -= blk->cnt, blk = blk->pre) {
                        if (n < blk->cnt) {
                            idx = blk->cnt - n - 1;
                            break;
                        }
                    }
                }
            }
        public:
            iterator() {}
            iterator(Block *_blk, int _idx): blk(_blk), idx(_idx) {}
            iterator operator+(const int &n) coprintf("~~~~~~\n");nst {
                iterator tmp = *this;
                tmp.plus(n);
                return tmp;
            }
            iterator operator-(const int &n) const {
                iterator tmp = *this;
                tmp.minus(n);
                return tmp;
            }
            // return th distance between two iterator,
            // if these two iterators points to different vectors, throw invaild_iterator.
            int operator-(const iterator &rhs) const {
                if (blk->deq != rhs.blk->deq) throw invalid_iterator();
                int n = idx;
                for (Block *cur = blk->pre; cur != nullptr; cur = cur->pre) n += cur->cnt;
                for (Block *cur = rhs.blk->pre; cur != nullptr; cur = cur->pre) n -= cur->cnt;
                return n - rhs.idx;
            }
            bool isEnd() {
                return blk == blk->deq->tail && idx == blk->deq->tail->cnt;
            }
            
            iterator operator+=(const int &n) {n >= 0 ? plus(n) : minus(-n);return *this;}
            iterator operator-=(const int &n) {n >= 0 ? minus(n) : plus(-n);return *this;}
            iterator operator++(int) {
                iterator tmp = *this;
                plus(1);
                return tmp;
            }
            iterator& operator++() {plus(1);return *this;}
            iterator operator--(int) {
                iterator tmp = *this;
                minus(1);
                return tmp;
            }
            iterator& operator--() {minus(1);return *this;}
            T& operator*() const {return *(blk->at(idx));}
            T* operator->() const noexcept {return blk->at(idx);}
            bool operator==(const iterator &rhs) const {return blk == rhs.blk && idx == rhs.idx;}
            bool operator==(const const_iterator &rhs) const {return blk == rhs.blk && idx == rhs.idx;}
            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {return !(*this == rhs);}
            bool operator!=(const const_iterator &rhs) const {return !(!this == rhs);}
        };
        class const_iterator {
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            
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
        };

        class Block {
        private:
            T* dat;
        public:
            size_t sz, cnt, base;
            Block *nxt, *pre;
            deque<T> *deq;
            inline T* at(int idx) {
                return dat + (idx + base + sz) % sz;
            }
            inline T* atArr(int idx, T* arr) {
                return arr + (idx + base + sz) % sz;
            }
            Block(deque<T> *_deq) {
                sz = 1;
                cnt = 0;
                dat = (T*)operator new(sizeof(T) * sz);
                deq = _deq;
                base = 0;
                nxt = pre = nullptr;
            }
            void doubleSpace() {
                T* tmp = dat;
                dat = (T*)operator new(sizeof(T) * sz * 2);
                for (int i = 0; i < cnt; i++) {
                    new(dat + i) T(*atArr(i, tmp));
                    atArr(i, tmp)->~T();
                }
                operator delete(tmp);
                base = 0;
                sz = sz * 2;
            }
            ~Block() {
//                std::cout << this << "   deleted" << std::endl;
                for (int i = 0; i < cnt; i++) {
                    at(i)->~T();
                }
                operator delete(dat);
            }

            void pushBack(const T& a) {
//                std::cout << this << " pushback  " << cnt << std::endl;
                new(at(cnt)) T(a);
                cnt = cnt + 1;
                if (cnt == sz) {
                    if (sz < bSize) doubleSpace();
                    else split(0);
                }
            }
            void pushFront(const T& a) {
//                std::cout << this << " pushfront" << std::endl;
                new(at(-1)) T(a);
                cnt = cnt + 1;
                base = (base - 1 + sz) % sz;
                if (cnt == sz) {
                    if (sz < bSize) doubleSpace();
                    else split(0);
                }
            }
            void popBack() {
//                std::cout << this << " popback" << std::endl;
                cnt = cnt - 1;
                at(cnt)->~T();
                if (this != deq->tail && cnt + nxt->cnt <= bSize / 2) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    pre->combi();
                }
            }
            void popFront() {
//                std::cout << this << " popfront " << cnt << " " << nxt->cnt << std::endl;
                at(0)->~T();
                cnt = cnt - 1;
                base = (base + 1) % sz;
                if (this != deq->tail && cnt + nxt->cnt <= bSize / 2) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    pre->combi();
                }
            }
            void print() {
//                for (int i = 0; i < cnt; i++) {
//                    std::cout << (*at(i)) << " ";
//                }
//                std::cout << std::endl;
            }
            iterator split(int idx) {
//                std::cout << "\nSPLIT!" << std::endl;
                Block *tmp = new Block(deq);
                tmp->nxt = nxt;
                tmp->pre = this;
                if (nxt != nullptr) nxt->pre = tmp;
                else deq->tail = tmp;
                nxt = tmp;
                for (size_t i = sz / 2; i < sz; i++) {
                    nxt->pushBack(*at(i));
                    at(i)->~T();
                }
                cnt -= (sz - sz / 2);
                
                if (idx < cnt) {
                    return iterator(this, idx);
                } else {
                    return iterator(nxt, idx - cnt);
                }
            }
            void combi() {
//                std::cout << "\nCOMBI!" << std::endl;
                for (int i = 0; i < nxt->cnt; i++) {
                    pushBack(*(nxt->at(i)));
                }
//                deq->print();
                if (nxt == deq->tail) deq->tail = this;
                else nxt->nxt->pre = this;
                Block *tmp = nxt;
                nxt = nxt->nxt;
                delete tmp;
            }
            iterator insAt(int idx, const T& a) {
                for (int i = cnt - 1; i >= idx; i--) {
                    new(at(i + 1)) T(*at(i));
                    at(i)->~T();
                }
                new(at(idx)) T(a);
                cnt = cnt + 1;
                if (cnt == sz) {
                    if (sz < bSize) doubleSpace();
                    else return split(idx);
                }
                return iterator(this, idx);
            }
            iterator delAt(int idx) {
                at(idx)->~T();
                for (int i = idx; i < cnt - 1; i++) {
                    new(at(i)) T(*at(i + 1));
                    at(i + 1)->~T();
                }
                cnt = cnt - 1;
                if (this != deq->tail && cnt + nxt->cnt <= bSize / 2) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    Block* tmp = pre;
                    tmp->combi();
                    return iterator(tmp, idx + tmp->cnt);
                } else return iterator(this, idx);
            }
        };
        deque() {
            head = tail = new Block(this);
            sz = 0;
        }
        deque(const deque &other) {copyFrom(other);}
        ~deque() {
//            print();
            for (Block *cur = head; cur != nullptr;) {
//                std::cout << cur << " " << cur->sz << std::endl;
                Block *tmp = cur;
                cur = cur -> nxt;
                delete tmp;
            }
        }
        deque &operator=(const deque &other) {
            if (&other == this) {
                return *this;
            }
            this->~deque<T>();
            copyFrom(other);
            return *this;
        }
        T & at(const size_t &pos) {
            size_t n = pos;
            for (Block *cur = head; cur != nullptr; n = n - cur->cnt, cur = cur -> nxt) {
                if (cur->cnt > n) return *(cur->at(n));
            }
            throw index_out_of_bound();
        }
        const T & at(const size_t &pos) const {return at(pos);}
        T & operator[](const size_t &pos) {return at(pos);}
        const T & operator[](const size_t &pos) const {return at(pos);}
        /**
         * access the first element
         * throw container_is_empty when the container is empty.
         */
        const T & front() const {
            if (empty()) throw container_is_empty();
            return *(head->at(0));
        }
        /**
         * access the last element
         * throw container_is_empty when the container is empty.
         */
        const T & back() const {
            if (empty()) throw container_is_empty();
            return *(tail->at(tail->cnt - 1));
        }
        /**
         * returns an iterator to the beginning.
         */
        iterator begin() {return iterator(head, 0);}
        const_iterator cbegin() const {}
        iterator end() {return iterator(tail, tail->cnt);}
        const_iterator cend() const {}
        
        bool empty() const {return sz == 0;}
        size_t size() const {return sz;}
        void clear() {
            this->~deque<T>();
            head = tail = new Block(this);
            sz = 0;
        }
        /**
         * inserts elements at the specified locat on in the container.
         * inserts value before pos
         * returns an iterator pointing to the inserted value
         *     throw if the iterator is invalid or it point to a wrong place.
         */
        iterator insert(iterator pos, const T &value) {
            sz++;
            if (pos.isEnd()) {
                tail->pushBack(value);
                return iterator(tail, tail->cnt - 1);
            } else {
                return pos.blk->insAt(pos.idx, value);
            }
        }
        /**
         * removes specified element at pos.
         * removes the element at pos.
         * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
         * throw if the container is empty, the iterator is invalid or it points to a wrong place.
         */
        iterator erase(iterator pos) {
            sz--;
            return pos.blk->delAt(pos.idx);
        }
        void push_back(const T &value) {
            sz++;
            tail->pushBack(value);
        }
        void pop_back() {
            sz--;
            tail->popBack();
        }
        void push_front(const T &value) {
            sz++;
            head->pushFront(value);
        }
        void pop_front() {
            sz--;
            head->popFront();
        }
        void print() {
            std::cout << head << " _____ " << tail << std::endl;
            for (Block *cur = head; cur != nullptr; cur = cur->nxt) {
                std::cout << cur << "  " << cur->nxt << "  " << cur->cnt << "  " << cur->sz << std::endl;
//                cur->print();
            }
            std::cout << std::endl;
        }
    };
    
}

#endif
