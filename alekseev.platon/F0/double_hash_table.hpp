#ifndef ALEKSEEV_F0_DOUBLE_HASH_TABLE_HPP
#define ALEKSEEV_F0_DOUBLE_HASH_TABLE_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace alekseev
{
  namespace detail
  {
    const std::size_t MinDoubleHashSlots = 11;
    const std::size_t DefaultDoubleHashSlots = 101;
  }

  enum class DoubleHashSlotState
  {
    EMPTY,
    OCCUPIED,
    DELETED
  };

  template< class Key, class Value, class Hash, class Equal >
  class DoubleHashTable
  {
  public:
    class iterator;
    class const_iterator;

    explicit DoubleHashTable(std::size_t slots = detail::DefaultDoubleHashSlots,
        Hash hash = Hash(), Equal equal = Equal());
    DoubleHashTable(const DoubleHashTable& other);
    DoubleHashTable(DoubleHashTable&& other) noexcept;
    ~DoubleHashTable();

    DoubleHashTable& operator=(const DoubleHashTable& other);
    DoubleHashTable& operator=(DoubleHashTable&& other) noexcept;

    void add(const Key& key, const Value& value);
    void add(Key&& key, Value&& value);
    bool has(const Key& key) const;
    Value& at(const Key& key);
    const Value& at(const Key& key) const;
    Value drop(const Key& key);
    void rehash(std::size_t slots);
    void clear();
    bool empty() const noexcept;
    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    std::size_t tombstoneCount() const noexcept;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    class iterator
    {
    public:
      struct Item
      {
        const Key& key() const;
        Value& value() const;

        const Key* key_;
        Value* value_;
      };

      iterator();
      iterator(DoubleHashTable* table, std::size_t index);

      iterator& operator++();
      Item operator*() const;
      const Item* operator->() const;
      bool operator==(const iterator& other) const;
      bool operator!=(const iterator& other) const;

    private:
      void skipToOccupied();

      DoubleHashTable* table_;
      std::size_t index_;
      mutable Item item_;
    };

    class const_iterator
    {
    public:
      struct Item
      {
        const Key& key() const;
        const Value& value() const;

        const Key* key_;
        const Value* value_;
      };

      const_iterator();
      const_iterator(const DoubleHashTable* table, std::size_t index);

      const_iterator& operator++();
      Item operator*() const;
      const Item* operator->() const;
      bool operator==(const const_iterator& other) const;
      bool operator!=(const const_iterator& other) const;

    private:
      void skipToOccupied();

      const DoubleHashTable* table_;
      std::size_t index_;
      mutable Item item_;
    };

  private:
    struct Slot
    {
      Slot();

      DoubleHashSlotState state;
      Key key;
      Value value;
    };

    Slot* slots_;
    std::size_t size_;
    std::size_t tombstones_;
    std::size_t capacity_;
    Hash hash_;
    Equal equal_;

    void swap(DoubleHashTable& other) noexcept;
  };
}

#include "double_hash_table-impl.hpp"

#endif
