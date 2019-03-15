#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
    
    template <class T>
    void swap(T &x, T &y) {
        T z = y;
        y = x;
        x = z;
    }
    
    /**
     * a container like std::priority_queue which is a heap internal.
     * it should be based on the vector written by yourself.
     */
    template<class T, class Compare = std::less<T>>
    class priority_queue {
        
        class PQNode {
        public:
            T *key;
            int height;
            PQNode *left, *right;
            PQNode(const T &_key): height(1), left(nullptr), right(nullptr) {
                key = new T(_key);
            }
            PQNode(const PQNode &other) {
                key = new T(*(other.key));
                height = other.height;
                left = nullptr;
                right = nullptr;
            }
            ~PQNode() {
                delete key;
            }
        };
        
    private:
        PQNode *head;
        size_t Size;
        Compare cmp;
        
        void deconstruct(PQNode *cur) {
            if (cur->left != nullptr) {
                deconstruct(cur->left);
            }
            if (cur->right != nullptr) {
                deconstruct(cur->right);
            }
            delete cur;
        }
        PQNode *_merge(PQNode *x, PQNode *y) {
            if (x == nullptr) {
                return y;
            }
            if (y == nullptr) {
                return x;
            }
            if (y == nullptr || cmp(*(x->key), *(y->key))) {
                swap(x, y);
            }
            x->right = _merge(x->right, y);
            if (x->left == nullptr) {
                swap(x->left, x->right);
                x->height = 1;
            } else if(x->left->height < x->right->height) {
                swap(x->left, x->right);
            }
            if (x->right != nullptr) {
                x->height = x->right->height + 1;
            }
            return x;
        }
        void copyTree(PQNode *cur, PQNode *tar) {
            if (tar->left != nullptr) {
                cur->left = new PQNode(*(tar->left));
                copyTree(cur->left, tar->left);
            }
            if (tar->right != nullptr) {
                cur->right = new PQNode(*(tar->right));
                copyTree(cur->right, tar->right);
            }
        }
        
    public:
        /**
         * TODO constructors
         */
        priority_queue():head(nullptr), Size(0) {
        }
        priority_queue(const priority_queue &other) {
            Size = other.Size;
            head = new PQNode(*(other.head));
            copyTree(head, other.head);
        }
        /**
         * TODO deconstructor
         */
        priority_queue(T _key) {
            head = new PQNode(_key);
            Size = 1;
        }
        ~priority_queue() {
            if (head != nullptr) {
                deconstruct(head);
            }
        }
        priority_queue &operator=(const priority_queue &other) {
            if (other.head == head) {
                return *this;
            }
            if (head != nullptr) deconstruct(head);
            Size = other.Size;
            head = new PQNode(*(other.head));
            copyTree(head, other.head);
            return *this;
        }
        /**
         * get the top of the queue.
         * @return a reference of the top element.
         * throw container_is_empty if empty() returns true;
         */
        const T & top() const {
            if (empty()) {
                throw container_is_empty();
            }
            return *(head->key);
        }
        /**
         * TODO
         * push new element to the priority queue.
         */
        void push(const T &_key) {
            PQNode *newNode = new PQNode(_key);
            head = _merge(head, newNode);
            Size = Size + 1;
        }
        /**
         * TODO
         * delete the top element.
         * throw container_is_empty if empty() returns true;
         */
        void pop() {
            if (empty()) {
                throw container_is_empty();
            }
            PQNode *temp = head;
            head = _merge(head->left, head->right);
            Size = Size - 1;
            delete temp;
        }
        /**
         * return the number of the elements.
         */
        size_t size() const {
            return Size;
        }
        /**
         * check if the container has at least an element.
         * @return true if it is empty, false if it has at least an element.
         */
        bool empty() const {
            return Size == 0;
        }
        /**
         * return a merged priority_queue with at least O(logn) complexity.
         */
        void merge(priority_queue &other) {
            Size = Size + other.Size;
            head = _merge(head, other.head);
            other.Size = 0;
            other.head = nullptr;
        }
    };
    
}

#endif
