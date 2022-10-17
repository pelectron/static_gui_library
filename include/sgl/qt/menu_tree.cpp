#include "sgl/qt/menu_tree.hpp"

namespace sgl::qt {
  Node::Node(std::string_view name, Node::Type type, Node* parent)
      : parent_{parent}, name_(name), type_(type) {}

  Node::Node(std::string_view name, std::vector<Node*> children, Node::Type type, Node* parent)
      : parent_(parent), children_(std::move(children)), name_(name), type_(type) {
    for (auto& child : children_) {
      child->set_parent(this);
    }
  }

  Node::~Node() {
    for (Node* child : children_) {
      delete child;
    }
  }

  Node*       Node::parent() { return parent_; }
  const Node* Node::parent() const { return parent_; }

  void Node::set_parent(Node* p) {
    assert(parent_ == nullptr);
    parent_ = p;
  }

  size_t Node::index() const {
    if (not parent_) {
      return 0;
    }
    
    const auto& siblings = parent_->children_;

    for (size_t i = 0; i < siblings.size(); ++i) {
      if (siblings[i] == this) {
        return i;
      }
    }
    return ~size_t{0};
  }

  Node::iterator            Node::begin() { return children_.begin(); }
  Node::iterator            Node::end() { return children_.end(); }
  Node::const_iterator      Node::begin() const { return children_.begin(); }
  Node::const_iterator      Node::end() const { return children_.end(); }
  std::vector<Node*>&       Node::children() { return children_; }
  const std::vector<Node*>& Node::children() const { return children_; }

  size_t           Node::size() const { return children_.size(); }
  std::string_view Node::name() const { return name_; }
  std::string_view Node::text() const { return {}; }
  std::string_view Node::type_name() const { return {}; }
  Node::Type       Node::type() const { return type_; }

  AbstractPageNode* AbstractItemNode::get_page() {
    return dynamic_cast<AbstractPageNode*>(parent());
  }

  const AbstractPageNode* AbstractItemNode::get_page() const {
    return dynamic_cast<const AbstractPageNode*>(parent());
  }

  bool AbstractItemNode::is_current() const { return get_page()->current_index() == this->index(); }
  
  AbstractMenuNode* AbstractPageNode::get_menu() {
    return dynamic_cast<AbstractMenuNode*>(parent());
  }

  AbstractItemNode* AbstractPageNode::current_item() {
    return dynamic_cast<AbstractItemNode*>(children()[current_index()]);
  }

  const AbstractItemNode* AbstractPageNode::current_item() const {
    return dynamic_cast<const AbstractItemNode*>(children()[current_index()]);
  }

  AbstractPageNode* AbstractMenuNode::current_page() {
    return dynamic_cast<AbstractPageNode*>(children()[current_index()]);
  }
  const AbstractPageNode* AbstractMenuNode::current_page() const {
    return dynamic_cast<const AbstractPageNode*>(children()[current_index()]);
  }
  AbstractItemNode* AbstractMenuNode::current_item() { return current_page()->current_item(); }
  const AbstractItemNode* AbstractMenuNode::current_item() const {
    return current_page()->current_item();
  }

  MenuIndex AbstractMenuNode::current_menu_index() const {
    return {current_index(), current_page()->current_index()};
  }

  MenuTree::MenuTree(MenuTree&& other) : root_(other.root_) { other.root_ = nullptr; }

  MenuTree::~MenuTree() {
    if (root_) {
      delete root_;
    }
  }

  AbstractMenuNode*       MenuTree::root() { return root_; }
  const AbstractMenuNode* MenuTree::root() const { return root_; }

  MenuIndex MenuTree::active_index() const { return root_->current_menu_index(); }

  AbstractItemNode* MenuTree::current_item() { return root_->current_item(); }

  AbstractPageNode* MenuTree::current_page() { return root_->current_page(); }
} // namespace sgl::qt