#ifndef ALEKSEEV_F0_DOUBLE_HASH_TABLE_IMPL_HPP
#define ALEKSEEV_F0_DOUBLE_HASH_TABLE_IMPL_HPP

#include "double_hash_table.hpp"

#include <utility>

namespace alekseev
{
  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::Slot::Slot():
    state(DoubleHashSlotState::EMPTY),
    key(),
    value()
  {
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::DoubleHashTable(std::size_t slots,
      Hash hash, Equal equal):
    slots_(nullptr),
    size_(0),
    tombstones_(0),
    capacity_(normalizeCapacity(slots)),
    hash_(hash),
    equal_(equal)
  {
    slots_ = new Slot[capacity_];
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::DoubleHashTable(const DoubleHashTable& other):
    DoubleHashTable(other.capacity_, other.hash_, other.equal_)
  {
    try
    {
      for (const_iterator it = other.cbegin(); it != other.cend(); ++it)
      {
        add(it->key(), it->value());
      }
    }
    catch (...)
    {
      delete[] slots_;
      slots_ = nullptr;
      throw;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::DoubleHashTable(DoubleHashTable&& other)
      noexcept:
    slots_(other.slots_),
    size_(other.size_),
    tombstones_(other.tombstones_),
    capacity_(other.capacity_),
    hash_(std::move(other.hash_)),
    equal_(std::move(other.equal_))
  {
    other.slots_ = nullptr;
    other.size_ = 0;
    other.tombstones_ = 0;
    other.capacity_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::~DoubleHashTable()
  {
    delete[] slots_;
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >&
      DoubleHashTable< Key, Value, Hash, Equal >::operator=(const DoubleHashTable& other)
  {
    if (this != &other)
    {
      DoubleHashTable copy(other);
      swap(copy);
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >&
      DoubleHashTable< Key, Value, Hash, Equal >::operator=(DoubleHashTable&& other) noexcept
  {
    if (this != &other)
    {
      delete[] slots_;
      slots_ = other.slots_;
      size_ = other.size_;
      tombstones_ = other.tombstones_;
      capacity_ = other.capacity_;
      hash_ = std::move(other.hash_);
      equal_ = std::move(other.equal_);
      other.slots_ = nullptr;
      other.size_ = 0;
      other.tombstones_ = 0;
      other.capacity_ = 0;
    }
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::add(const Key& key, const Value& value)
  {
    insertNew(key, value);
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::add(Key&& key, Value&& value)
  {
    insertNew(std::move(key), std::move(value));
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::has(const Key& key) const
  {
    return findSlot(key) != nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& DoubleHashTable< Key, Value, Hash, Equal >::at(const Key& key)
  {
    Slot* slot = findSlot(key);
    if (slot == nullptr)
    {
      throw std::out_of_range("double hash key");
    }
    return slot->value;
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& DoubleHashTable< Key, Value, Hash, Equal >::at(const Key& key) const
  {
    const Slot* slot = findSlot(key);
    if (slot == nullptr)
    {
      throw std::out_of_range("double hash key");
    }
    return slot->value;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value DoubleHashTable< Key, Value, Hash, Equal >::drop(const Key& key)
  {
    Slot* slot = findSlot(key);
    if (slot == nullptr)
    {
      throw std::out_of_range("double hash key");
    }
    Value value(std::move(slot->value));
    slot->state = DoubleHashSlotState::DELETED;
    --size_;
    ++tombstones_;
    return value;
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::rehash(std::size_t slots)
  {
    DoubleHashTable next(slots, hash_, equal_);
    for (const_iterator it = cbegin(); it != cend(); ++it)
    {
      next.add(it->key(), it->value());
    }
    swap(next);
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::clear()
  {
    for (std::size_t i = 0; i < capacity_; ++i)
    {
      slots_[i].state = DoubleHashSlotState::EMPTY;
    }
    size_ = 0;
    tombstones_ = 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::empty() const noexcept
  {
    return size_ == 0;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::size() const noexcept
  {
    return size_;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::capacity() const noexcept
  {
    return capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::tombstoneCount() const noexcept
  {
    return tombstones_;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::iterator
      DoubleHashTable< Key, Value, Hash, Equal >::begin()
  {
    return iterator(this, 0);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::iterator
      DoubleHashTable< Key, Value, Hash, Equal >::end()
  {
    return iterator(this, capacity_);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator
      DoubleHashTable< Key, Value, Hash, Equal >::begin() const
  {
    return const_iterator(this, 0);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator
      DoubleHashTable< Key, Value, Hash, Equal >::end() const
  {
    return const_iterator(this, capacity_);
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator
      DoubleHashTable< Key, Value, Hash, Equal >::cbegin() const
  {
    return begin();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator
      DoubleHashTable< Key, Value, Hash, Equal >::cend() const
  {
    return end();
  }

  template< class Key, class Value, class Hash, class Equal >
  const Key& DoubleHashTable< Key, Value, Hash, Equal >::iterator::Item::key() const
  {
    return *key_;
  }

  template< class Key, class Value, class Hash, class Equal >
  Value& DoubleHashTable< Key, Value, Hash, Equal >::iterator::Item::value() const
  {
    return *value_;
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::iterator::iterator():
    table_(nullptr),
    index_(0),
    item_{nullptr, nullptr}
  {
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::iterator::iterator(DoubleHashTable* table,
      std::size_t index):
    table_(table),
    index_(index),
    item_{nullptr, nullptr}
  {
    skipToOccupied();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::iterator&
      DoubleHashTable< Key, Value, Hash, Equal >::iterator::operator++()
  {
    ++index_;
    skipToOccupied();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::iterator::Item
      DoubleHashTable< Key, Value, Hash, Equal >::iterator::operator*() const
  {
    return Item{&table_->slots_[index_].key, &table_->slots_[index_].value};
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename DoubleHashTable< Key, Value, Hash, Equal >::iterator::Item*
      DoubleHashTable< Key, Value, Hash, Equal >::iterator::operator->() const
  {
    item_ = **this;
    return &item_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::iterator::operator==(
      const iterator& other) const
  {
    return table_ == other.table_ && index_ == other.index_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::iterator::operator!=(
      const iterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::iterator::skipToOccupied()
  {
    while (table_ != nullptr && index_ < table_->capacity_ &&
        table_->slots_[index_].state != DoubleHashSlotState::OCCUPIED)
    {
      ++index_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  const Key& DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::Item::key()
      const
  {
    return *key_;
  }

  template< class Key, class Value, class Hash, class Equal >
  const Value& DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::Item::value()
      const
  {
    return *value_;
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::const_iterator():
    table_(nullptr),
    index_(0),
    item_{nullptr, nullptr}
  {
  }

  template< class Key, class Value, class Hash, class Equal >
  DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::const_iterator(
      const DoubleHashTable* table, std::size_t index):
    table_(table),
    index_(index),
    item_{nullptr, nullptr}
  {
    skipToOccupied();
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator&
      DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::operator++()
  {
    ++index_;
    skipToOccupied();
    return *this;
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::Item
      DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::operator*() const
  {
    return Item{&table_->slots_[index_].key, &table_->slots_[index_].value};
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::Item*
      DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::operator->() const
  {
    item_ = **this;
    return &item_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::operator==(
      const const_iterator& other) const
  {
    return table_ == other.table_ && index_ == other.index_;
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::operator!=(
      const const_iterator& other) const
  {
    return !(*this == other);
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::const_iterator::skipToOccupied()
  {
    while (table_ != nullptr && index_ < table_->capacity_ &&
        table_->slots_[index_].state != DoubleHashSlotState::OCCUPIED)
    {
      ++index_;
    }
  }

  template< class Key, class Value, class Hash, class Equal >
  void DoubleHashTable< Key, Value, Hash, Equal >::swap(DoubleHashTable& other) noexcept
  {
    using std::swap;
    swap(slots_, other.slots_);
    swap(size_, other.size_);
    swap(tombstones_, other.tombstones_);
    swap(capacity_, other.capacity_);
    swap(hash_, other.hash_);
    swap(equal_, other.equal_);
  }

  template< class Key, class Value, class Hash, class Equal >
  template< class K, class V >
  void DoubleHashTable< Key, Value, Hash, Equal >::insertNew(K&& key, V&& value)
  {
    if (capacity_ == 0)
    {
      throw std::overflow_error("double hash capacity");
    }

    Slot* firstDeleted = nullptr;
    for (std::size_t i = 0; i < capacity_; ++i)
    {
      Slot& slot = slots_[probeIndex(key, i)];
      if (slot.state == DoubleHashSlotState::OCCUPIED)
      {
        if (equal_(slot.key, key))
        {
          throw std::logic_error("duplicate double hash key");
        }
      }
      else if (slot.state == DoubleHashSlotState::DELETED)
      {
        if (firstDeleted == nullptr)
        {
          firstDeleted = &slot;
        }
      }
      else
      {
        Slot& target = firstDeleted == nullptr ? slot : *firstDeleted;
        target.key = std::forward< K >(key);
        target.value = std::forward< V >(value);
        if (target.state == DoubleHashSlotState::DELETED)
        {
          --tombstones_;
        }
        target.state = DoubleHashSlotState::OCCUPIED;
        ++size_;
        return;
      }
    }

    if (firstDeleted != nullptr)
    {
      firstDeleted->key = std::forward< K >(key);
      firstDeleted->value = std::forward< V >(value);
      firstDeleted->state = DoubleHashSlotState::OCCUPIED;
      --tombstones_;
      ++size_;
      return;
    }
    throw std::overflow_error("double hash table full");
  }

  template< class Key, class Value, class Hash, class Equal >
  typename DoubleHashTable< Key, Value, Hash, Equal >::Slot*
      DoubleHashTable< Key, Value, Hash, Equal >::findSlot(const Key& key)
  {
    return const_cast< Slot* >(
        static_cast< const DoubleHashTable& >(*this).findSlot(key));
  }

  template< class Key, class Value, class Hash, class Equal >
  const typename DoubleHashTable< Key, Value, Hash, Equal >::Slot*
      DoubleHashTable< Key, Value, Hash, Equal >::findSlot(const Key& key) const
  {
    if (capacity_ == 0)
    {
      return nullptr;
    }

    for (std::size_t i = 0; i < capacity_; ++i)
    {
      const Slot& slot = slots_[probeIndex(key, i)];
      if (slot.state == DoubleHashSlotState::EMPTY)
      {
        return nullptr;
      }
      if (slot.state == DoubleHashSlotState::OCCUPIED && equal_(slot.key, key))
      {
        return &slot;
      }
    }
    return nullptr;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::probeIndex(const Key& key,
      std::size_t probe) const noexcept
  {
    const std::size_t first = hash_(key) % capacity_;
    return (first + probe * secondHash(key)) % capacity_;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::secondHash(const Key& key)
      const noexcept
  {
    if (capacity_ <= 2)
    {
      return 1;
    }
    return 1 + (hash_(key) / capacity_) % (capacity_ - 1);
  }

  template< class Key, class Value, class Hash, class Equal >
  bool DoubleHashTable< Key, Value, Hash, Equal >::isPrime(std::size_t value) noexcept
  {
    if (value < 2)
    {
      return false;
    }
    if (value == 2)
    {
      return true;
    }
    if (value % 2 == 0)
    {
      return false;
    }
    for (std::size_t divisor = 3; divisor * divisor <= value; divisor += 2)
    {
      if (value % divisor == 0)
      {
        return false;
      }
    }
    return true;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::nextPrime(std::size_t value)
      noexcept
  {
    while (!isPrime(value))
    {
      ++value;
    }
    return value;
  }

  template< class Key, class Value, class Hash, class Equal >
  std::size_t DoubleHashTable< Key, Value, Hash, Equal >::normalizeCapacity(
      std::size_t slots) noexcept
  {
    if (slots < detail::MinDoubleHashSlots)
    {
      slots = detail::MinDoubleHashSlots;
    }
    return nextPrime(slots);
  }
}

#endif
