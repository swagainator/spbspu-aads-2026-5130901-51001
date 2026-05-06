#ifndef ALEKSEEV_BSTREE_IMPL_HPP
#define ALEKSEEV_BSTREE_IMPL_HPP

#include <utility>
#include <bstree.hpp>

namespace alekseev
{
  namespace detail
  {
    inline BaseNode::BaseNode():
      parent(nullptr),
      left(nullptr),
      right(nullptr)
    {
    }

    template< class Key, class Value >
    TreeNode< Key, Value >::TreeNode(const Key& key, const Value& value):
      BaseNode(),
      data(key, value)
    {
    }
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator():
    node_(nullptr)
  {
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >::BSTIterator(detail::BaseNode* node):
    node_(node)
  {
  }

  template< class Key, class Value >
  std::pair< Key, Value >& BSTIterator< Key, Value >::operator*() const
  {
    return static_cast< Node* >(node_)->data;
  }

  template< class Key, class Value >
  std::pair< Key, Value >* BSTIterator< Key, Value >::operator->() const
  {
    return &static_cast< Node* >(node_)->data;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >& BSTIterator< Key, Value >::operator++()
  {
    node_ = BSTree< Key, Value >::next(node_);
    return *this;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > BSTIterator< Key, Value >::operator++(int)
  {
    BSTIterator current(*this);
    ++(*this);
    return current;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value >& BSTIterator< Key, Value >::operator--()
  {
    node_ = BSTree< Key, Value >::prev(node_);
    return *this;
  }

  template< class Key, class Value >
  BSTIterator< Key, Value > BSTIterator< Key, Value >::operator--(int)
  {
    BSTIterator current(*this);
    --(*this);
    return current;
  }

  template< class Key, class Value >
  bool BSTIterator< Key, Value >::operator==(const BSTIterator& other) const
  {
    return node_ == other.node_;
  }

  template< class Key, class Value >
  bool BSTIterator< Key, Value >::operator!=(const BSTIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator():
    node_(nullptr)
  {
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(const BSTIterator< Key, Value >& other):
    node_(other.node_)
  {
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >::BSTConstIterator(const detail::BaseNode* node):
    node_(node)
  {
  }

  template< class Key, class Value >
  const std::pair< Key, Value >& BSTConstIterator< Key, Value >::operator*() const
  {
    return static_cast< const Node* >(node_)->data;
  }

  template< class Key, class Value >
  const std::pair< Key, Value >* BSTConstIterator< Key, Value >::operator->() const
  {
    return &static_cast< const Node* >(node_)->data;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >& BSTConstIterator< Key, Value >::operator++()
  {
    node_ = BSTree< Key, Value >::next(node_);
    return *this;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > BSTConstIterator< Key, Value >::operator++(int)
  {
    BSTConstIterator current(*this);
    ++(*this);
    return current;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value >& BSTConstIterator< Key, Value >::operator--()
  {
    node_ = BSTree< Key, Value >::prev(node_);
    return *this;
  }

  template< class Key, class Value >
  BSTConstIterator< Key, Value > BSTConstIterator< Key, Value >::operator--(int)
  {
    BSTConstIterator current(*this);
    --(*this);
    return current;
  }

  template< class Key, class Value >
  bool BSTConstIterator< Key, Value >::operator==(const BSTConstIterator& other) const
  {
    return node_ == other.node_;
  }

  template< class Key, class Value >
  bool BSTConstIterator< Key, Value >::operator!=(const BSTConstIterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
    fakeRoot_(),
    size_(0),
    cmp_(Compare())
  {
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(Compare cmp):
    fakeRoot_(),
    size_(0),
    cmp_(cmp)
  {
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(const BSTree& other):
    fakeRoot_(),
    size_(0),
    cmp_(other.cmp_)
  {
    fakeRoot_.left = cloneSubtree(other.fakeRoot_.left, &fakeRoot_);
    size_ = other.size_;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(BSTree&& other) noexcept:
    fakeRoot_(),
    size_(other.size_),
    cmp_(std::move(other.cmp_))
  {
    fakeRoot_.left = other.fakeRoot_.left;
    if (fakeRoot_.left != nullptr)
    {
      fakeRoot_.left->parent = &fakeRoot_;
    }
    other.fakeRoot_.left = nullptr;
    other.size_ = 0;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::~BSTree()
  {
    clear();
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >& BSTree< Key, Value, Compare >::operator=(const BSTree& other)
  {
    if (this != &other)
    {
      BSTree copy(other);
      swap(copy);
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >& BSTree< Key, Value, Compare >::operator=(BSTree&& other) noexcept
  {
    if (this != &other)
    {
      clear();
      fakeRoot_.left = other.fakeRoot_.left;
      if (fakeRoot_.left != nullptr)
      {
        fakeRoot_.left->parent = &fakeRoot_;
      }
      size_ = other.size_;
      cmp_ = std::move(other.cmp_);
      other.fakeRoot_.left = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::size() const noexcept
  {
    return size_;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::clear() noexcept
  {
    deleteSubtree(fakeRoot_.left);
    fakeRoot_.left = nullptr;
    size_ = 0;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::swap(BSTree& other) noexcept
  {
    using std::swap;
    swap(fakeRoot_.left, other.fakeRoot_.left);
    swap(size_, other.size_);
    swap(cmp_, other.cmp_);
    if (fakeRoot_.left != nullptr)
    {
      fakeRoot_.left->parent = &fakeRoot_;
    }
    if (other.fakeRoot_.left != nullptr)
    {
      other.fakeRoot_.left->parent = &other.fakeRoot_;
    }
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::push(const Key& key, const Value& value)
  {
    detail::BaseNode* parent = &fakeRoot_;
    detail::BaseNode** link = &fakeRoot_.left;
    while (*link != nullptr)
    {
      parent = *link;
      Node* node = static_cast< Node* >(*link);
      if (cmp_(key, node->data.first))
      {
        link = &(*link)->left;
      }
      else if (cmp_(node->data.first, key))
      {
        link = &(*link)->right;
      }
      else
      {
        node->data.second = value;
        return;
      }
    }
    Node* node = new Node(key, value);
    node->parent = parent;
    *link = node;
    ++size_;
  }

  template< class Key, class Value, class Compare >
  Value& BSTree< Key, Value, Compare >::get(const Key& key)
  {
    detail::BaseNode* node = findNode(key);
    if (node == nullptr)
    {
      throw std::out_of_range("bst key");
    }
    return static_cast< Node* >(node)->data.second;
  }

  template< class Key, class Value, class Compare >
  const Value& BSTree< Key, Value, Compare >::get(const Key& key) const
  {
    const detail::BaseNode* node = findNode(key);
    if (node == nullptr)
    {
      throw std::out_of_range("bst key");
    }
    return static_cast< const Node* >(node)->data.second;
  }

  template< class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::contains(const Key& key) const
  {
    return findNode(key) != nullptr;
  }

  template< class Key, class Value, class Compare >
  Value BSTree< Key, Value, Compare >::drop(const Key& key)
  {
    detail::BaseNode* node = findNode(key);
    if (node == nullptr)
    {
      throw std::out_of_range("bst key");
    }
    Value value = static_cast< Node* >(node)->data.second;
    if (node->left == nullptr)
    {
      transplant(node, node->right);
    }
    else if (node->right == nullptr)
    {
      transplant(node, node->left);
    }
    else
    {
      detail::BaseNode* successor = minimum(node->right);
      if (successor->parent != node)
      {
        transplant(successor, successor->right);
        successor->right = node->right;
        successor->right->parent = successor;
      }
      transplant(node, successor);
      successor->left = node->left;
      successor->left->parent = successor;
    }
    delete static_cast< Node* >(node);
    --size_;
    return value;
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator BSTree< Key, Value, Compare >::begin() noexcept
  {
    return iterator(empty() ? &fakeRoot_ : minimum(fakeRoot_.left));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator BSTree< Key, Value, Compare >::end() noexcept
  {
    return iterator(&fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
      BSTree< Key, Value, Compare >::begin() const noexcept
  {
    return cbegin();
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
      BSTree< Key, Value, Compare >::end() const noexcept
  {
    return cend();
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
      BSTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return const_iterator(empty() ? &fakeRoot_ : minimum(fakeRoot_.left));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iterator
      BSTree< Key, Value, Compare >::cend() const noexcept
  {
    return const_iterator(&fakeRoot_);
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height() const noexcept
  {
    return nodeHeight(fakeRoot_.left);
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::height(const_iterator it) const noexcept
  {
    if (it == cend())
    {
      return 0;
    }
    return nodeHeight(it.node_);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateLeft(const_iterator it)
  {
    return rotateLeftNode(const_cast< detail::BaseNode* >(it.node_));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateRight(const_iterator it)
  {
    return rotateRightNode(const_cast< detail::BaseNode* >(it.node_));
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateLargeLeft(const_iterator it)
  {
    detail::BaseNode* node = const_cast< detail::BaseNode* >(it.node_);
    if (node == nullptr || node->parent == nullptr || node->parent->parent == nullptr ||
        node->parent->parent == &fakeRoot_ || node->parent->left != node ||
        node->parent->parent->right != node->parent)
    {
      throw std::logic_error("large left rotation");
    }
    rotateRightNode(node);
    return rotateLeftNode(node);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateLargeRight(const_iterator it)
  {
    detail::BaseNode* node = const_cast< detail::BaseNode* >(it.node_);
    if (node == nullptr || node->parent == nullptr || node->parent->parent == nullptr ||
        node->parent->parent == &fakeRoot_ || node->parent->right != node ||
        node->parent->parent->left != node->parent)
    {
      throw std::logic_error("large right rotation");
    }
    rotateLeftNode(node);
    return rotateRightNode(node);
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::minimum(detail::BaseNode* node) noexcept
  {
    while (node != nullptr && node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  const detail::BaseNode* BSTree< Key, Value, Compare >::minimum(
      const detail::BaseNode* node) noexcept
  {
    while (node != nullptr && node->left != nullptr)
    {
      node = node->left;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::maximum(detail::BaseNode* node) noexcept
  {
    while (node != nullptr && node->right != nullptr)
    {
      node = node->right;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  const detail::BaseNode* BSTree< Key, Value, Compare >::maximum(
      const detail::BaseNode* node) noexcept
  {
    while (node != nullptr && node->right != nullptr)
    {
      node = node->right;
    }
    return node;
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::next(detail::BaseNode* node) noexcept
  {
    if (node->right != nullptr)
    {
      return minimum(node->right);
    }
    detail::BaseNode* parent = node->parent;
    while (parent != nullptr && node == parent->right)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  const detail::BaseNode* BSTree< Key, Value, Compare >::next(
      const detail::BaseNode* node) noexcept
  {
    if (node->right != nullptr)
    {
      return minimum(node->right);
    }
    const detail::BaseNode* parent = node->parent;
    while (parent != nullptr && node == parent->right)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::prev(detail::BaseNode* node) noexcept
  {
    if (node->left != nullptr)
    {
      return maximum(node->left);
    }
    detail::BaseNode* parent = node->parent;
    while (parent != nullptr && node == parent->left)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  const detail::BaseNode* BSTree< Key, Value, Compare >::prev(
      const detail::BaseNode* node) noexcept
  {
    if (node->left != nullptr)
    {
      return maximum(node->left);
    }
    const detail::BaseNode* parent = node->parent;
    while (parent != nullptr && node == parent->left)
    {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  template< class Key, class Value, class Compare >
  size_t BSTree< Key, Value, Compare >::nodeHeight(const detail::BaseNode* node) noexcept
  {
    if (node == nullptr)
    {
      return 0;
    }
    const size_t left = nodeHeight(node->left);
    const size_t right = nodeHeight(node->right);
    return (left > right ? left : right) + 1;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::deleteSubtree(detail::BaseNode* node) noexcept
  {
    if (node == nullptr)
    {
      return;
    }
    deleteSubtree(node->left);
    deleteSubtree(node->right);
    delete static_cast< Node* >(node);
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::findNode(const Key& key) noexcept
  {
    detail::BaseNode* node = fakeRoot_.left;
    while (node != nullptr)
    {
      Node* current = static_cast< Node* >(node);
      if (cmp_(key, current->data.first))
      {
        node = node->left;
      }
      else if (cmp_(current->data.first, key))
      {
        node = node->right;
      }
      else
      {
        return node;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  const detail::BaseNode* BSTree< Key, Value, Compare >::findNode(const Key& key) const noexcept
  {
    const detail::BaseNode* node = fakeRoot_.left;
    while (node != nullptr)
    {
      const Node* current = static_cast< const Node* >(node);
      if (cmp_(key, current->data.first))
      {
        node = node->left;
      }
      else if (cmp_(current->data.first, key))
      {
        node = node->right;
      }
      else
      {
        return node;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Compare >
  detail::BaseNode* BSTree< Key, Value, Compare >::cloneSubtree(
      const detail::BaseNode* node, detail::BaseNode* parent)
  {
    if (node == nullptr)
    {
      return nullptr;
    }
    const Node* source = static_cast< const Node* >(node);
    Node* copy = new Node(source->data.first, source->data.second);
    copy->parent = parent;
    try
    {
      copy->left = cloneSubtree(node->left, copy);
      copy->right = cloneSubtree(node->right, copy);
    }
    catch (...)
    {
      deleteSubtree(copy);
      throw;
    }
    return copy;
  }

  template< class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::transplant(
      detail::BaseNode* node, detail::BaseNode* replacement) noexcept
  {
    if (node == node->parent->left)
    {
      node->parent->left = replacement;
    }
    else
    {
      node->parent->right = replacement;
    }
    if (replacement != nullptr)
    {
      replacement->parent = node->parent;
    }
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateLeftNode(detail::BaseNode* node)
  {
    if (node == nullptr || node == &fakeRoot_ || node->parent == nullptr ||
        node->parent == &fakeRoot_ || node->parent->right != node)
    {
      throw std::logic_error("left rotation");
    }
    detail::BaseNode* parent = node->parent;
    detail::BaseNode* grandparent = parent->parent;
    parent->right = node->left;
    if (node->left != nullptr)
    {
      node->left->parent = parent;
    }
    node->parent = grandparent;
    if (grandparent->left == parent)
    {
      grandparent->left = node;
    }
    else
    {
      grandparent->right = node;
    }
    node->left = parent;
    parent->parent = node;
    return iterator(node);
  }

  template< class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iterator
      BSTree< Key, Value, Compare >::rotateRightNode(detail::BaseNode* node)
  {
    if (node == nullptr || node == &fakeRoot_ || node->parent == nullptr ||
        node->parent == &fakeRoot_ || node->parent->left != node)
    {
      throw std::logic_error("right rotation");
    }
    detail::BaseNode* parent = node->parent;
    detail::BaseNode* grandparent = parent->parent;
    parent->left = node->right;
    if (node->right != nullptr)
    {
      node->right->parent = parent;
    }
    node->parent = grandparent;
    if (grandparent->left == parent)
    {
      grandparent->left = node;
    }
    else
    {
      grandparent->right = node;
    }
    node->right = parent;
    parent->parent = node;
    return iterator(node);
  }
}

#endif
