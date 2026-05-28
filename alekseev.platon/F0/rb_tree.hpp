#ifndef ALEKSEEV_F0_RB_TREE_HPP
#define ALEKSEEV_F0_RB_TREE_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>

namespace alekseev
{
  namespace detail
  {
    enum class RBColor
    {
      RED,
      BLACK
    };

    template< class Key, class Value >
    struct RBNode
    {
      RBNode(const Key& key, const Value& value);

      std::pair< Key, Value > data;
      RBNode* left;
      RBNode* right;
      RBNode* parent;
      RBColor color;
    };
  }

  template< class Key, class Value >
  class RBTreeIterator;

  template< class Key, class Value >
  class RBTreeConstIterator;

  template< class Key, class Value >
  class RBTreeIterator
  {
  public:
    using Node = detail::RBNode< Key, Value >;

    RBTreeIterator();

    std::pair< Key, Value >& operator*() const;
    std::pair< Key, Value >* operator->() const;
    RBTreeIterator& operator++();
    RBTreeIterator operator++(int);
    bool operator==(const RBTreeIterator& other) const;
    bool operator!=(const RBTreeIterator& other) const;

  private:
    explicit RBTreeIterator(Node* node);

    Node* node_;

    template< class K, class V, class Compare >
    friend class RBTree;
    template< class K, class V >
    friend class RBTreeConstIterator;
  };

  template< class Key, class Value >
  class RBTreeConstIterator
  {
  public:
    using Node = detail::RBNode< Key, Value >;

    RBTreeConstIterator();
    RBTreeConstIterator(const RBTreeIterator< Key, Value >& other);

    const std::pair< Key, Value >& operator*() const;
    const std::pair< Key, Value >* operator->() const;
    RBTreeConstIterator& operator++();
    RBTreeConstIterator operator++(int);
    bool operator==(const RBTreeConstIterator& other) const;
    bool operator!=(const RBTreeConstIterator& other) const;

  private:
    explicit RBTreeConstIterator(const Node* node);

    const Node* node_;

    template< class K, class V, class Compare >
    friend class RBTree;
  };

  template< class Key, class Value, class Compare = std::less< Key > >
  class RBTree
  {
  public:
    using iterator = RBTreeIterator< Key, Value >;
    using const_iterator = RBTreeConstIterator< Key, Value >;

    RBTree();
    explicit RBTree(Compare cmp);
    RBTree(const RBTree& other);
    RBTree(RBTree&& other) noexcept;
    ~RBTree();

    RBTree& operator=(const RBTree& other);
    RBTree& operator=(RBTree&& other) noexcept;

    bool empty() const noexcept;
    std::size_t size() const noexcept;
    void clear() noexcept;
    void swap(RBTree& other) noexcept;

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

    std::size_t height() const noexcept;
    bool checkInvariants() const noexcept;

  private:
    friend class RBTreeIterator< Key, Value >;
    friend class RBTreeConstIterator< Key, Value >;

    using Node = detail::RBNode< Key, Value >;

    Node* root_;
    std::size_t size_;
    Compare cmp_;

    Node* findNode(const Key& key) noexcept;
    const Node* findNode(const Key& key) const noexcept;
    static Node* minimum(Node* node) noexcept;
    static const Node* minimum(const Node* node) noexcept;
    static Node* next(Node* node) noexcept;
    static const Node* next(const Node* node) noexcept;
    static std::size_t nodeHeight(const Node* node) noexcept;
    static void deleteSubtree(Node* node) noexcept;
  };
}

#include "rb_tree-impl.hpp"

#endif
