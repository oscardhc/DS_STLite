#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <iostream>

namespace sjtu {
    
    const size_t bSize = 512;
    
    template<class T>
    class deque {
    public:
        class Block;
    private:
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
            friend iterator deque::insert(iterator pos, const T &value);
            friend iterator deque::erase(iterator pos);
        private:
            Block *blk;
            int idx;
            void plus(int n) {
                if (blk->cnt - idx > n || (blk == blk->deq->tail && blk->cnt - idx == n)) idx += n;
                else {
                    n -= (blk->cnt - idx);
                    for (blk = blk->nxt; blk != nullptr; n -= blk->cnt, blk = blk->nxt) {
                        if (n < blk->cnt || (blk == blk->deq->tail && n <= blk->cnt)) {
                            idx = n;
                            return;
                        }
                    }
                    throw runtime_error();
                }
            }
            void minus(int n) {
                if (idx >= n) {
                    idx -= n;
                } else {
                    n -= (idx + 1);
                    for (blk = blk->pre; blk != nullptr; n -= blk->cnt, blk = blk->pre) {
                        if (n < blk->cnt) {
                            idx = blk->cnt - n - 1;
                            return;
                        }
                    }
                    throw runtime_error();
                }
            }
        public:
            iterator() {}
            iterator(Block *_blk, int _idx): blk(_blk), idx(_idx) {}
            iterator operator+(const int &n) const {
                iterator tmp = *this;
                n >= 0? tmp.plus(n) : tmp.minus(-n);
                return tmp;
            }
            iterator operator-(const int &n) const {
                iterator tmp = *this;
                n >= 0 ? tmp.minus(n) : tmp.plus(-n);
                return tmp;
            }
            int operator-(const iterator &rhs) const {
                if (blk->deq != rhs.blk->deq) throw invalid_iterator();
                int n = idx;
                for (Block *cur = blk->pre; cur != nullptr; cur = cur->pre) n += cur->cnt;
                for (Block *cur = rhs.blk->pre; cur != nullptr; cur = cur->pre) n -= cur->cnt;
                return n - rhs.idx;
            }
            bool isEnd() const {
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
            T& operator*() const {
                if (isEnd()) throw runtime_error();
                return *(blk->at(idx));
            }
            T* operator->() const noexcept {
                if (isEnd()) throw runtime_error();
                return (blk->at(idx));
            }
            bool operator==(const iterator &rhs) const {
                return blk == rhs.blk && idx == rhs.idx;
            }
            bool operator!=(const iterator &rhs) const {return !(*this == rhs);}
        };
        using const_iterator = iterator;
        
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
                for (int i = 0; i < cnt; i++) {
                    at(i)->~T();
                }
                operator delete(dat);
            }
            
            void pushBack(const T& a) {
                new(at(cnt)) T(a);
                cnt = cnt + 1;
                if (cnt == sz) {
                    if (sz < bSize) doubleSpace();
                    else split(0);
                }
            }
            void pushFront(const T& a) {
                new(at(-1)) T(a);
                cnt = cnt + 1;
                base = (base - 1 + sz) % sz;
                if (cnt == sz) {
                    if (sz < bSize) doubleSpace();
                    else split(0);
                }
            }
            void popBack() {
                cnt = cnt - 1;
                at(cnt)->~T();
                if (this != deq->tail && cnt + nxt->cnt <= bSize * 2 / 3) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    pre->combi();
                }
            }
            void popFront() {
                at(0)->~T();
                cnt = cnt - 1;
                base = (base + 1) % sz;
                if (this != deq->tail && (cnt + nxt->cnt <= bSize * 2 / 3 || cnt == 0)) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    pre->combi();
                }
            }
            iterator split(int idx) {
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
                for (int i = 0; i < nxt->cnt; i++) {
                    pushBack(*(nxt->at(i)));
                }
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
                if (this != deq->tail && cnt + nxt->cnt <= bSize * 2 / 3) combi();
                if (this == deq->tail && cnt == 0 && this != deq->head) {
                    Block* tmp = pre;
                    tmp->combi();
                    return iterator(tmp, idx + tmp->cnt);
                } else {
                    if (idx < cnt || this == deq->tail) return iterator(this, idx);
                    else return iterator(nxt, 0);
                }
            }
        };
        deque() {
            head = tail = new Block(this);
            sz = 0;
        }
        deque(const deque &other) {copyFrom(other);}
        ~deque() {
            for (Block *cur = head; cur != nullptr;) {
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
        const T & at(const size_t &pos) const {
            size_t n = pos;
            for (Block *cur = head; cur != nullptr; n = n - cur->cnt, cur = cur -> nxt) {
                if (cur->cnt > n) return *(cur->at(n));
            }
            throw index_out_of_bound();
        }
        T & operator[](const size_t &pos) {return at(pos);}
        const T & operator[](const size_t &pos) const {return at(pos);}
        const T & front() const {
            if (empty()) throw container_is_empty();
            return *(head->at(0));
        }
        const T & back() const {
            if (empty()) throw container_is_empty();
            return *(tail->at(tail->cnt - 1));
        }
        iterator begin() {return iterator(head, 0);}
        const_iterator cbegin() const {return const_iterator(head, 0);}
        iterator end() {return iterator(tail, tail->cnt);}
        const_iterator cend() const {return const_iterator(tail, tail->cnt);}
        
        bool empty() const {return sz == 0;}
        size_t size() const {return sz;}
        void clear() {
            this->~deque<T>();
            head = tail = new Block(this);
            sz = 0;
        }
        iterator insert(iterator pos, const T &value) {
            if (pos.blk->deq != this) throw runtime_error();
            sz++;
            if (pos.isEnd()) {
                tail->pushBack(value);
                return iterator(tail, tail->cnt - 1);
            } else {
                return pos.blk->insAt(pos.idx, value);
            }
        }
        iterator erase(iterator pos) {
            if (pos.blk->deq != this || pos.isEnd()) throw runtime_error();
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
    };
    
}

#endif
