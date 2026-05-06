#ifndef ALEKSEEV_SEQUENCE_HPP
#define ALEKSEEV_SEQUENCE_HPP

#include <cstddef>
#include <new>
#include <stdexcept>
#include <utility>

namespace alekseev
{
  namespace detail
  {
    const size_t InitialSequenceCapacity = 4;
    const size_t SequenceGrowthFactor = 2;
  }

  template< class T >
  class Sequence
  {
  public:
    Sequence():
      data_(nullptr),
      size_(0),
      capacity_(0)
    {
    }

    Sequence(const Sequence& other):
      data_(nullptr),
      size_(0),
      capacity_(0)
    {
      reserve(other.size_);
      try
      {
        for (size_t i = 0; i < other.size_; ++i)
        {
          new (data_ + i) T(other.data_[i]);
          ++size_;
        }
      }
      catch (...)
      {
        clear();
        operator delete(data_);
        data_ = nullptr;
        capacity_ = 0;
        throw;
      }
    }

    Sequence(Sequence&& other) noexcept:
      data_(other.data_),
      size_(other.size_),
      capacity_(other.capacity_)
    {
      other.data_ = nullptr;
      other.size_ = 0;
      other.capacity_ = 0;
    }

    ~Sequence()
    {
      clear();
      operator delete(data_);
    }

    Sequence& operator=(const Sequence& other)
    {
      if (this != &other)
      {
        Sequence copy(other);
        swap(copy);
      }
      return *this;
    }

    Sequence& operator=(Sequence&& other) noexcept
    {
      if (this != &other)
      {
        clear();
        operator delete(data_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
      }
      return *this;
    }

    void push_back(const T& value)
    {
      ensureCapacityForOneMore();
      new (data_ + size_) T(value);
      ++size_;
    }

    void push_back(T&& value)
    {
      ensureCapacityForOneMore();
      new (data_ + size_) T(std::move(value));
      ++size_;
    }

    void erase_at(size_t index)
    {
      if (index >= size_)
      {
        throw std::out_of_range("sequence index");
      }
      data_[index].~T();
      for (size_t i = index; i + 1 < size_; ++i)
      {
        new (data_ + i) T(std::move(data_[i + 1]));
        data_[i + 1].~T();
      }
      --size_;
    }

    void clear() noexcept
    {
      for (size_t i = 0; i < size_; ++i)
      {
        data_[i].~T();
      }
      size_ = 0;
    }

    T& operator[](size_t index)
    {
      return data_[index];
    }

    const T& operator[](size_t index) const
    {
      return data_[index];
    }

    size_t size() const noexcept
    {
      return size_;
    }

    bool empty() const noexcept
    {
      return size_ == 0;
    }

    T* data() noexcept
    {
      return data_;
    }

    const T* data() const noexcept
    {
      return data_;
    }

    void reserve(size_t new_capacity)
    {
      if (new_capacity <= capacity_)
      {
        return;
      }

      T* new_data = static_cast< T* >(operator new(sizeof(T) * new_capacity));
      size_t constructed = 0;
      try
      {
        for (; constructed < size_; ++constructed)
        {
          new (new_data + constructed) T(data_[constructed]);
        }
      }
      catch (...)
      {
        for (size_t i = 0; i < constructed; ++i)
        {
          new_data[i].~T();
        }
        operator delete(new_data);
        throw;
      }

      for (size_t i = 0; i < size_; ++i)
      {
        data_[i].~T();
      }
      operator delete(data_);
      data_ = new_data;
      capacity_ = new_capacity;
    }

    void swap(Sequence& other) noexcept
    {
      using std::swap;
      swap(data_, other.data_);
      swap(size_, other.size_);
      swap(capacity_, other.capacity_);
    }

  private:
    void ensureCapacityForOneMore()
    {
      if (size_ == capacity_)
      {
        const size_t next = capacity_ == 0 ? detail::InitialSequenceCapacity :
            capacity_ * detail::SequenceGrowthFactor;
        reserve(next);
      }
    }

    T* data_;
    size_t size_;
    size_t capacity_;
  };
}

#endif
