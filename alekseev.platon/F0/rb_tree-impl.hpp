#ifndef ALEKSEEV_F0_RB_TREE_IMPL_HPP
#define ALEKSEEV_F0_RB_TREE_IMPL_HPP

#include "rb_tree.hpp"

#include <utility>

namespace alekseev
{
  namespace detail
  {
    template< class Key, class Value >
    RBNode< Key, Value >::RBNode(const Key& key, const Value& value):
      data(key, value),
      left(nullptr),
      right(nullptr),
      parent(nullptr),
      color(RBColor::RED)
    {
    }
  }

  template< class Key, class Value >
  RBTreeIterator< Key, Value >::RBTreeIterator():
    node_(nullptr)
  {
  }

  template< class Key, class Value >
  RBTreeIterator< Key, Value >::RBTreeIterator(Node* node):
    node_(node)
  {
  }

  template< class Key, class Value >
  std::pair< Key, Value >& RBTreeIterator< Key, Value >::operator*() const
  {
    return node_->data;
  }

  template< class Key, class Value >
  std::pair< Key, Value >* RBTreeIterator< Key, Value >::operator->() const
  {
    return std::addressof(node_->data);
  }

  template< class Key, class Value >
  RBTreeIterator< Key, Value >& RBTreeIterator< Key, Value >::operator++()
  {
    node_ = RBTree< Key, Value >::next(node_);
    return *this;
  }

  template< class Key, class Value >
  RBTreeIterator< Key, Value > RBTreeIterator< Key, Value >::operator++(int)
  {
    RBTreeIterator current(*this);
    ++(*this);
    return current;
  }

  template< class Key, class Value >
  bool RBTreeIterator< Key, Value >::operator==(const RBTreeIterator& other) const
  {
    return node_ == other.node_;
  }

  template< class Key, class Value >
  bool RBTreeIterator< Key, Value >::operator!=(const RBTreeIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  RBTreeConstIterator< Key, Value >::RBTreeConstIterator():
    node_(nullptr)
  {
  }

  template< class Key, class Value >
  RBTreeConstIterator< Key, Value >::RBTreeConstIterator(
      const RBTreeIterator< Key, Value >& other):
    node_(other.node_)
  {
  }

  template< class Key, class Value >
  RBTreeConstIterator< Key, Value >::RBTreeConstIterator(const Node* node):
    node_(node)
  {
  }

  template< class Key, class Value >
  const std::pair< Key, Value >& RBTreeConstIterator< Key, Value >::operator*() const
  {
    return node_->data;
  }

  template< class Key, class Value >
  const std::pair< Key, Value >* RBTreeConstIterator< Key, Value >::operator->() const
  {
    return std::addressof(node_->data);
  }

  template< class Key, class Value >
  RBTreeConstIterator< Key, Value >& RBTreeConstIterator< Key, Value >::operator++()
  {
    node_ = RBTree< Key, Value >::next(node_);
    return *this;
  }

  template< class Key, class Value >
  RBTreeConstIterator< Key, Value > RBTreeConstIterator< Key, Value >::operator++(int)
  {
    RBTreeConstIterator current(*this);
    ++(*this);
    return current;
  }

  template< class Key, class Value >
  bool RBTreeConstIterator< Key, Value >::operator==(const RBTreeConstIterator& other) const
  {
    return node_ == other.node_;
  }

  template< class Key, class Value >
  bool RBTreeConstIterator< Key, Value >::operator!=(const RBTreeConstIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >::RBTree():
    root_(nullptr),
    size_(0),
    cmp_(Compare())
  {
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >::RBTree(Compare cmp):
    root_(nullptr),
    size_(0),
    cmp_(cmp)
  {
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >::RBTree(const RBTree& other):
    RBTree(other.cmp_)
  {
    for (const_iterator it = other.cbegin(); it != other.cend(); ++it)
    {
      push(it->first, it->second);
    }
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >::RBTree(RBTree&& other) noexcept:
    root_(other.root_),
    size_(other.size_),
    cmp_(std::move(other.cmp_))
  {
    other.root_ = nullptr;
    other.size_ = 0;
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >::~RBTree()
  {
    clear();
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >& RBTree< Key, Value, Compare >::operator=(
      const RBTree& other)
  {
    if (this != &other)
    {
      RBTree copy(other);
      swap(copy);
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  RBTree< Key, Value, Compare >& RBTree< Key, Value, Compare >::operator=(
      RBTree&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      root_ = other.root_;
      size_ = other.size_;
      cmp_ = std::move(other.cmp_);
      other.root_ = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  bool RBTree< Key, Value, Compare >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Compare >
  std::size_t RBTree< Key, Value, Compare >::size() const noexcept
  {
    return size_;
  }

  template< class Key, class Value, class Compare >
  void RBTree< Key, Value, Compare >::clear() noexcept
  {
    deleteSubtree(root_);
    root_ = nullptr;
    size_ = 0;
  }

  template< class Key, class Value, class Compare >
  void RBTree< Key, Value, Compare >::swap(RBTree& other) noexcept
  {
    std::swap(root_, other.root_);
    std::swap(size_, other.size_);
    std::swap(cmp_, other.cmp_);
  }

  template< class Key, class Value, class Compare >
  void RBTree< Key, Value, Compare >::push(const Key&, const Value&)
  {
    throw std::logic_error("rb tree insertion is not implemented");
  }

  template< class Key, class Value, class Compare >
  Value& RBTree< Key, Value, Compare >::get(const Key& key)
  {
    Node* node = findNode(key);
    if (node == nullptr)
    {
      throw std::out_of_range("rb tree key");
    }
    return node->data.second;
  }

  template< class Key, class Value, class Compare >
  const Value& RBTree< Key, Value, Compare >::get(const Key& key) const
  {
    const Node* node = findNode(key);
    if (node == nullptr)
    {
      throw std::out_of_range("rb tree key");
    }
    return node->data.second;
  }

  template< class Key, class Value, class Compare >
  bool RBTree< Key, Value, Compare >::contains(const Key& key) const
  {
    return findNode(key) != nullptr;
  }

  template< class Key, class Value, class Compare >
  Value RBTree< Key, Value, Compare >::drop(const Key&)
  {
    throw std::logic_error("rb tree erase is not implemented");
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::iterator RBTree< Key, Value, Compare >::begin()
      noexcept
  {
    return iterator(minimum(root_));
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::iterator RBTree< Key, Value, Compare >::end()
      noexcept
  {
    return iterator(nullptr);
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::const_iterator
      RBTree< Key, Value, Compare >::begin() const noexcept
  {
    return const_iterator(minimum(root_));
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::const_iterator
      RBTree< Key, Value, Compare >::end() const noexcept
  {
    return const_iterator(nullptr);
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::const_iterator
      RBTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return begin();
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::const_iterator
      RBTree< Key, Value, Compare >::cend() const noexcept
  {
    return end();
  }

  template< class Key, class Value, class Compare >
  std::size_t RBTree< Key, Value, Compare >::height() const noexcept
  {
    return nodeHeight(root_);
  }

  template< class Key, class Value, class Compare >
  bool RBTree< Key, Value, Compare >::checkInvariants() const noexcept
  {
    return root_ == nullptr || root_->color == detail::RBColor::BLACK;
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::findNode(const Key& key) noexcept
  {
    Node* current = root_;
    while (current != nullptr)
    {
      if (cmp_(key, current->data.first))
      {
        current = current->left;
      }
      else if (cmp_(current->data.first, key))
      {
        current = current->right;
      }
      else
      {
        return current;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  const typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::findNode(const Key& key) const noexcept
  {
    const Node* current = root_;
    while (current != nullptr)
    {
      if (cmp_(key, current->data.first))
      {
        current = current->left;
      }
      else if (cmp_(current->data.first, key))
      {
        current = current->right;
      }
      else
      {
        return current;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::minimum(Node* node) noexcept
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    while (node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  const typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::minimum(const Node* node) noexcept
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    while (node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::next(Node* node) noexcept
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    if (node->right != nullptr)
    {
      return minimum(node->right);
    }
    Node* parent = node->parent;
    while (parent != nullptr && node == parent->right)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  const typename RBTree< Key, Value, Compare >::Node*
      RBTree< Key, Value, Compare >::next(const Node* node) noexcept
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    if (node->right != nullptr)
    {
      return minimum(node->right);
    }
    const Node* parent = node->parent;
    while (parent != nullptr && node == parent->right)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  std::size_t RBTree< Key, Value, Compare >::nodeHeight(const Node* node) noexcept
  {
    if (node == nullptr)
    {
      return 0;
    }
    const std::size_t left = nodeHeight(node->left);
    const std::size_t right = nodeHeight(node->right);
    return (left < right ? right : left) + 1;
  }

  template< class Key, class Value, class Compare >
  void RBTree< Key, Value, Compare >::deleteSubtree(Node* node) noexcept
  {
    if (node == nullptr)
    {
      return;
    }
    deleteSubtree(node->left);
    deleteSubtree(node->right);
    delete node;
  }
}

#endif
