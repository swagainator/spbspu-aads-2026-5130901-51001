#ifndef ALEKSEEV_BSTREE_HPP
#define ALEKSEEV_BSTREE_HPP

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>

namespace alekseev
{
  namespace detail
  {
    struct BaseNode
    {
      BaseNode();

      BaseNode* parent;
      BaseNode* left;
      BaseNode* right;
    };

    template< class Key, class Value >
    struct TreeNode: BaseNode
    {
      TreeNode(const Key& key, const Value& value);

      std::pair< Key, Value > data;
    };
  }

  template< class Key, class Value >
  class BSTIterator;

  template< class Key, class Value >
  class BSTConstIterator;

  template< class Key, class Value >
  class BSTIterator
  {
  public:
    using Node = detail::TreeNode< Key, Value >;

    BSTIterator();

    std::pair< Key, Value >& operator*() const;
    std::pair< Key, Value >* operator->() const;
    BSTIterator& operator++();
    BSTIterator operator++(int);
    BSTIterator& operator--();
    BSTIterator operator--(int);
    bool operator==(const BSTIterator& other) const;
    bool operator!=(const BSTIterator& other) const;

  private:
    explicit BSTIterator(detail::BaseNode* node);

    detail::BaseNode* node_;

    template< class K, class V, class Compare >
    friend class BSTree;
    template< class K, class V >
    friend class BSTConstIterator;
  };

  template< class Key, class Value >
  class BSTConstIterator
  {
  public:
    using Node = detail::TreeNode< Key, Value >;

    BSTConstIterator();
    BSTConstIterator(const BSTIterator< Key, Value >& other);

    const std::pair< Key, Value >& operator*() const;
    const std::pair< Key, Value >* operator->() const;
    BSTConstIterator& operator++();
    BSTConstIterator operator++(int);
    BSTConstIterator& operator--();
    BSTConstIterator operator--(int);
    bool operator==(const BSTConstIterator& other) const;
    bool operator!=(const BSTConstIterator& other) const;

  private:
    explicit BSTConstIterator(const detail::BaseNode* node);

    const detail::BaseNode* node_;

    template< class K, class V, class Compare >
    friend class BSTree;
  };

  template< class Key, class Value, class Compare = std::less< Key > >
  class BSTree
  {
  public:
    using iterator = BSTIterator< Key, Value >;
    using const_iterator = BSTConstIterator< Key, Value >;

    BSTree();
    explicit BSTree(Compare cmp);
    BSTree(const BSTree& other);
    BSTree(BSTree&& other) noexcept;
    ~BSTree();

    BSTree& operator=(const BSTree& other);
    BSTree& operator=(BSTree&& other) noexcept;

    bool empty() const noexcept;
    size_t size() const noexcept;
    void clear() noexcept;
    void swap(BSTree& other) noexcept;

    void push(const Key& key, const Value& value);
    Value& get(const Key& key);
    const Value& get(const Key& key) const;
    bool contains(const Key& key) const;
    Value drop(const Key& key);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    size_t height() const noexcept;
    size_t height(const_iterator it) const noexcept;
    iterator rotateLeft(const_iterator it);
    iterator rotateRight(const_iterator it);
    iterator rotateLargeLeft(const_iterator it);
    iterator rotateLargeRight(const_iterator it);

  private:
    friend class BSTIterator< Key, Value >;
    friend class BSTConstIterator< Key, Value >;

    using Node = detail::TreeNode< Key, Value >;

    static detail::BaseNode* minimum(detail::BaseNode* node) noexcept;
    static const detail::BaseNode* minimum(const detail::BaseNode* node) noexcept;
    static detail::BaseNode* maximum(detail::BaseNode* node) noexcept;
    static const detail::BaseNode* maximum(const detail::BaseNode* node) noexcept;
    static detail::BaseNode* next(detail::BaseNode* node) noexcept;
    static const detail::BaseNode* next(const detail::BaseNode* node) noexcept;
    static detail::BaseNode* prev(detail::BaseNode* node) noexcept;
    static const detail::BaseNode* prev(const detail::BaseNode* node) noexcept;
    static size_t nodeHeight(const detail::BaseNode* node) noexcept;
    static void deleteSubtree(detail::BaseNode* node) noexcept;

    detail::BaseNode* findNode(const Key& key) noexcept;
    const detail::BaseNode* findNode(const Key& key) const noexcept;
    detail::BaseNode* cloneSubtree(const detail::BaseNode* node, detail::BaseNode* parent);
    void transplant(detail::BaseNode* node, detail::BaseNode* replacement) noexcept;
    iterator rotateLeftNode(detail::BaseNode* node);
    iterator rotateRightNode(detail::BaseNode* node);

    detail::BaseNode fakeRoot_;
    size_t size_;
    Compare cmp_;
  };
}

#include <bstree-impl.hpp>

#endif
