#pragma once

#include <functional>

#include "Engine/Math.h"

template<typename KeyType, typename ValueType>
class Tree2D
{
public:
    struct Point
    {
        KeyType x, y;
        ValueType value;
    };

    struct Node
    {
        struct D1
        {
            Node* left = nullptr;
            Node* right = nullptr;
        };
    
        Point point;
        D1 left;
        D1 right;

        explicit Node(const Point& point)
            : point(point)
        {
        }
    };

private:
    static Point* find(std::function<bool(const Point& point)> predicate, Node* node)
    {
        if (node)
        {
            if (auto left_left = find(predicate, node->left.left)) return left_left;
            if (auto left_right = find(predicate, node->left.right)) return left_right;
            if (predicate(node->point)) return &node->point;
            if (auto right_left = find(predicate, node->right.left)) return right_left;
            if (auto right_right = find(predicate, node->right.right)) return right_right;
        }

        return nullptr;
    }

    /*static void remove(KeyType x, KeyType y, Node*& node)
    {
        if (node)
        {
            if (x == node->point.x && y == node->point.y)
            {
                auto subs = take_smallest(node->right.right);
                
                delete node;
                return;
            }

            typename Node::D1& d1 = (x < node->point.x) ? node->left : node->right;
            Node*& n = (y < node->point.y) ? d1.left : d1.right;
        
            remove(x, y, n);
        }
    }*/

    /*static Node* take_smallest(Node*& node)
    {
        if (node)
        {
            if (auto child = take_smallest(node->left.left)) return child;
            auto node_copy = node;
            node = nullptr;
            return node_copy;
        }

        return nullptr;
    }*/

    static void for_each(std::function<void(const Point& point)> callback, Node* node)
    {
        if (node)
        {
            for_each(callback, node->left.left);
            for_each(callback, node->left.right);
            callback(node->point);
            for_each(callback, node->right.left);
            for_each(callback, node->right.right);
        }
    }

    static uint height(Node* node)
    {
        if (node)
        {
            return Math::max(Math::max(height(node->left.left), height(node->left.right)), Math::max(node->right.left), height(node->right.right)) + 1;
        }
        
        return 0;
    }

public:
    void insert(KeyType x, KeyType y, ValueType value)
    {
        Node** current = &root_;

        while (*current != nullptr)
        {
            if (x == (*current)->point.key && y == (*current)->point.y)
            {
                (*current)->point = Point(x, y, value);
                return;
            }

            typename Node::D1& d1 = (x < (*current)->point.key) ? (*current)->left : (*current)->right;
            current = (y < (*current)->point.y) ? &d1.left : &d1.right;
        }
        
        *current = new Node(Point(x, y, value));
        ++size_;
    }

    const Point* find(KeyType x, KeyType y)
    {
        Node* current = root_;

        while (current != nullptr)
        {
            if (x == current->point.key && y == current->point.y)
            {
                return &current->point;
            }

            typename Node::D1& d1 = (x < current->point.key) ? current->left : current->right;
            current = (y < current->point.y) ? d1.left : d1.right;
        }

        return nullptr;
    }

    const Point* find(std::function<bool(const Point& point)> callback)
    {
        return find(callback, root_);
    }

    void remove(KeyType x, KeyType y)
    {
        //remove(x, y, root);
    }

    void for_each(std::function<void(const Point& point)> callback)
    {
        for_each(callback, root_);
    }

private:
    Node* root_ = nullptr;
    uint size_ = 0;
};
