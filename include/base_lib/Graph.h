#pragma once

#include "Map.h"

template<typename ValueType>
class Graph {
    class Node {
        friend Graph;

    public:
        const List<Node*>& get_links() const { return links_; }
        void link(Node* node) {
            if (!links_.contains(node)) {
                links_.add(node);
                node->links_.add(this);
            }
        }
        void unlink(Node* node) {
            if (uint index = links_.index_of(node) != BAD_INDEX) {
                links_.remove_at(index);
                node->links_.remove(this);
            }
        }
        ValueType value;

    private:
        List<Node*> links_;
    };

    ~Graph() {
        for (auto node : nodes_) {
            delete node;
        }
    }

    Node* new_node() {
        return nodes_.insert(new Node());
    }

    uint size() const { return nodes_.size(); }

    const Tree1D<Node*>& get_all_nodes() const { return nodes_; }

private:
    Tree1D<Node*> nodes_;
};