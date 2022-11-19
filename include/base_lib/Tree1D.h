#pragma once

#include <functional>

#include "Math.h"

template<typename PointType>
class Tree1D
{
public:
    struct Node
    {
        PointType point;
        Node* left = nullptr;
        Node* right = nullptr;
        int h = 1;

        Node(const Node& rhs)
            : point(rhs.point)
            , left(rhs.left)
            , right(rhs.right)
            , h(rhs.h)
        {
        }
        
        explicit Node(const PointType& point)
            : point(point)
        {
        }

        void update_h()
        {
            h = Math::max(left ? left->h : 0, right ? right->h : 0) + 1;
        }

        void write_to_stream(std::ostream& stream) const requires Serializable<PointType>
        {
            StreamUtils::write(stream, point);
            StreamUtils::write(stream, (char)(!!left << 1 | !!right));
            if (left) left->write_to_stream(stream);
            if (right) right->write_to_stream(stream);
        }

        void read_from_stream(std::istream& stream) requires Serializable<PointType>
        {
            StreamUtils::read(stream, point);
            const char state = StreamUtils::read<char>(stream);
            if (state & 2)
            {
                if (!left) left = new Node();
                left->read_from_stream(stream);
                
            }
            else if (left) Tree1D::destroy(left);
            if (state & 1)
            {
                if (!right) right = new Node();
                right->read_from_stream(stream);
            }
            else if (right) Tree1D::destroy(right);

            update_h();
        }
    };

    class Iterator
    {
        friend Tree1D;
    
    public:
        bool operator==(const Iterator& rhs) const
        {
            return tree_ == rhs.tree_ && i_ == rhs.i_;
        }

        bool operator!=(const Iterator& rhs) const
        {
            return tree_ != rhs.tree_ || i_ != rhs.i_;
        }

        Iterator& operator++()
        {
            i_++;
            if (stack_[stack_i_ - 1]->right != nullptr)
            {
                stack_[stack_i_++] = stack_[stack_i_ - 1]->right;
                while (stack_[stack_i_ - 1]->left != nullptr)
                {
                    stack_[stack_i_++] = stack_[stack_i_ - 1]->left;
                }
            }
            else
            {
                Node* prev;
                do
                {
                    prev = stack_[--stack_i_];
                }
                while (stack_i_ != 0 && prev == stack_[stack_i_ - 1]->right);
            }

            return *this;
        }

        PointType& operator*() const { return stack_[stack_i_ - 1]->point; }
        PointType& operator->() { return stack_[stack_i_ - 1]->point; }
   
        explicit Iterator(const Tree1D* tree)
            : Iterator(tree, 0)
        {
            if (tree->root_ != nullptr)
            {
                stack_ = new Node*[tree->root_->h + 1];
                stack_[stack_i_++] = tree->root_;
                while (stack_[stack_i_ - 1]->left != nullptr)
                {
                    stack_[stack_i_++] = stack_[stack_i_ - 1]->left;
                }
            }
        }

        ~Iterator()
        {
            delete[] stack_;
        }

        static Iterator get_end(const Tree1D* tree)
        {
            return Iterator(tree, tree->size_);
        }

    private:
        Iterator(const Tree1D* tree, uint i)
            : tree_(tree)
            , i_(i)
        {
        }

        const Tree1D* tree_;
        uint i_;
        Node** stack_ = nullptr;
        uint stack_i_ = 0;
    };

private:
    static void rotation_ll(Node*& node)
    {
        auto l_temp = node->left;
        auto lr_temp = l_temp->right;
        node->left->right = node;
        node->left = lr_temp;
        node = l_temp;

        if (node->left) node->left->update_h();
        if (node->right) node->right->update_h();
        node->update_h();
    }

    static void rotation_rr(Node*& node)
    {
        auto r_temp = node->right;
        auto rl_temp = node->right->left;
        node->right->left = node;
        node->right = rl_temp;
        node = r_temp;

        if (node->left) node->left->update_h();
        if (node->right) node->right->update_h();
        node->update_h();
    }

    static void rotation_rl(Node*& node)
    {
        auto rl_temp = node->right->left;
        auto rll_temp = node->right->left->left;

        node->right->left = node->right->left->right;
        rl_temp->right = node->right;
        node->right = rll_temp;
        rl_temp->left = node;
        node = rl_temp;

        if (node->left)
        {
            if (node->left->right) node->left->right->update_h();
            node->left->update_h();
        }
        if (node->right)
        {
            if (node->right->left) node->right->left->update_h();
            node->right->update_h();
        }
        node->update_h();
    }
    
    static void rotation_lr(Node*& node)
    {
        auto lr_temp = node->left->right;
        auto lrr_temp = node->left->right->right;

        node->left->right = node->left->right->left;
        lr_temp->left = node->left;
        node->left = lrr_temp;
        lr_temp->right = node;
        node = lr_temp;

        if (node->left)
        {
            if (node->left->right) node->left->right->update_h();
            node->left->update_h();
        }
        if (node->right)
        {
            if (node->right->left) node->right->left->update_h();
            node->right->update_h();
        }
        node->update_h();
    }

    static int bf(Node* node)
    {
        if (node)
        {
            return (node->left ? node->left->h : 0) - (node->right ? node->right->h : 0);
        }

        return 0;
    }
    
    static void balance(Node*& node)
    {
        auto bfs = bf(node);
        auto bfl = bf(node->left);
        auto bfr = bf(node->right);

        if (bfs == 2 && bfl == 1)
        {
            rotation_ll(node);
        }
        else if (bfs == -2 && bfr == -1)
        {
            rotation_rr(node);
        }
        else if (bfs == -2 && bfr == 1)
        {
            rotation_rl(node);
        }
        else if (bfs == 2 && bfl == -1)
        {
            rotation_lr(node);
        }
    }
    
    static int insert(const PointType& point, Node*& node, uint& size, Node*& new_node)
    {
        if (node)
        {
            if (point == node->point)
            {
                node->point = point;
                return node->h;
            }

            node->h = insert(point, (point < node->point) ? node->left : node->right, size, new_node) + 1;
            balance(node);
            return node->h;
        }
        else
        {
            node = new_node = new Node(point);
            size++;
            return node->h;
        }
    }

    static Node* take_most_right(Node*& node)
    {
        if (!node->right)
        {
            Node* result = node;
            node = node->left;
            result->left = nullptr;
            return result;
        }
        
        return take_most_right(node->right);
    }

    static Node* take_most_left(Node*& node)
    {
        if (!node->left)
        {
            Node* result = node;
            node = node->right;
            result->right = nullptr;
            return result;
        }
        
        return take_most_right(node->left);
    }
    
    static PointType* find(std::function<bool(const PointType& point)> predicate, Node* node)
    {
        if (node)
        {
            if (auto left = find(predicate, node->left)) return left;
            if (predicate(node->point)) return &node->point;
            if (auto right = find(predicate, node->right)) return right;
        }

        return nullptr;
    }

    static int remove(const PointType& point, Node*& node, uint& size)
    {
        if (node)
        {
            if (point == node->point)
            {
                size--;
                if (node->left)
                {
                    auto most_r = take_most_right(node->left);
                    most_r->left = node->left;
                    most_r->right = node->right;
                    delete node;
                    node = most_r;
                    return node->h;
                }
                else if (node->right)
                {
                    auto most_l = take_most_left(node->right);
                    most_l->right = node->right;
                    delete node;
                    node = most_l;
                    return node->h;
                }
                else
                {
                    delete node;
                    node = nullptr;
                    return 0;
                }
            }

            node->h = remove(point, (point < node->point) ? node->left : node->right, size) + 1;
            return node->h;
        }

        return 0;
    }

    static void for_each(std::function<void(const PointType& point)> callback, Node* node)
    {
        if (node)
        {
            for_each(callback, node->left);
            callback(node->point);
            for_each(callback, node->right);
        }
    }

    static void destroy(Node*& node)
    {
        if (node)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    static void copy(Node* from, Node*& to)
    {
        if (from)
        {
            to = new Node(*from);
            copy(from->left, to->left);
            copy(from->right, to->right);
        }
    }

public:
    Tree1D()
        : root_(nullptr)
        , size_(0)
    {
    }

    Tree1D(const Tree1D& rhs)
        : size_(rhs.size_)
    {
        copy(rhs.root_, root_);
    }

    Tree1D& operator=(const Tree1D& rhs)
    {
        if (this == &rhs) return *this;

        destroy(root_);
        copy(rhs.root_, root_);

        size_ = rhs.size_;

        return *this;
    }
    
    ~Tree1D()
    {
        destroy(root_);
    }

    void clear()
    {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    PointType& insert(const PointType& point)
    {
        Node* new_node;
        insert(point, root_, size_, new_node);
        return new_node->point;
    }

    PointType* find(const PointType& point_pattern) const
    {
        Node* current = root_;

        while (current != nullptr)
        {
            if (point_pattern == current->point)
            {
                return &current->point;
            }

            current = (point_pattern < current->point) ? current->left : current->right;
        }

        return nullptr;
    }

    PointType* find(std::function<bool(const PointType& point)> callback) const
    {
        return find(callback, root_);
    }

    void remove(const PointType& point)
    {
        remove(point, root_, size_);
    }

    void for_each(std::function<void(const PointType& point)> callback)
    {
        for_each(callback, root_);
    }

    Iterator begin()
    {
        return Iterator(this);
    }

    const Iterator begin() const
    {
        return Iterator(this);
    }

    Iterator end()
    {
        return Iterator::get_end(this);
    }
    
    const Iterator end() const
    {
        return Iterator::get_end(this);
    }

    uint size() const
    {
        return size_;
    }

    void write_to_stream(std::ostream& stream) const requires Serializable<PointType>
    {
        StreamUtils::write(root_ != nullptr);
        if (root_ != nullptr) root_->write_to_stream(stream);
    }

    void read_from_stream(std::istream& stream) requires Serializable<PointType>
    {
        if (StreamUtils::read<bool>(stream))
        {
            root_ = new Node();
            root_->read_from_stream(stream);
        }
    }

private:
    Node* root_;
    uint size_;
};
