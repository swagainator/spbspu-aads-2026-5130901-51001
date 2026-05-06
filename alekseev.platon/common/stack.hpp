#ifndef ALEKSEEV_STACK_HPP
#define ALEKSEEV_STACK_HPP

#include <cstddef>
#include <stdexcept>

#include <list.hpp>

namespace alekseev
{
  template< class T >
  class Stack
  {
  public:
    Stack() = default;

    Stack(const Stack< T >& other):
      data_(other.data_),
      size_(other.size_)
    {}

    Stack(Stack< T >&& other) noexcept:
      data_(static_cast< List< T >&& >(other.data_)),
      size_(other.size_)
    {
      other.size_ = 0;
    }

    Stack< T >& operator=(const Stack< T >& other)
    {
      if (this != std::addressof(other))
      {
        Stack tmp(other);
        swap(tmp);
      }
      return *this;
    }

    Stack< T >& operator=(Stack< T >&& other) noexcept
    {
      if (this != std::addressof(other))
      {
        data_ = static_cast< List< T >&& >(other.data_);
        size_ = other.size_;
        other.size_ = 0;
      }
      return *this;
    }

    ~Stack() = default;

    void swap(Stack< T >& other) noexcept
    {
      data_.swap(other.data_);
      std::size_t tmp = size_;
      size_ = other.size_;
      other.size_ = tmp;
    }

    void push(const T& value)
    {
      data_.push_front(value);
      ++size_;
    }

    void push(T&& value)
    {
      data_.push_front(static_cast< T&& >(value));
      ++size_;
    }

    T drop()
    {
      if (empty())
      {
        throw std::logic_error("stack is empty");
      }
      T value = static_cast< T&& >(data_.front());
      data_.pop_front();
      --size_;
      return value;
    }

    T& top()
    {
      if (empty())
      {
        throw std::logic_error("stack is empty");
      }
      return data_.front();
    }

    const T& top() const
    {
      if (empty())
      {
        throw std::logic_error("stack is empty");
      }
      return data_.front();
    }

    bool empty() const
    {
      return size_ == 0;
    }

    std::size_t size() const
    {
      return size_;
    }

  private:
    List< T > data_;
    std::size_t size_ = 0;
  };
}

#endif
