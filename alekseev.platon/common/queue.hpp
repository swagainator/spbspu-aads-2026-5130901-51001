#ifndef ALEKSEEV_QUEUE_HPP
#define ALEKSEEV_QUEUE_HPP

#include <cstddef>
#include <stdexcept>

#include <list.hpp>

namespace alekseev
{
  template< class T >
  class Queue
  {
  public:
    Queue():
      data_(),
      tail_(data_.before_begin()),
      size_(0)
    {}

    Queue(const Queue< T >& other):
      data_(other.data_),
      tail_(data_.before_begin()),
      size_(other.size_)
    {
      if (!data_.empty())
      {
        auto it = data_.before_begin();
        auto next = data_.begin();
        while (next != data_.end())
        {
          it = next;
          ++next;
        }
        tail_ = it;
      }
    }

    Queue(Queue< T >&& other) noexcept:
      data_(static_cast< List< T >&& >(other.data_)),
      tail_(data_.before_begin()),
      size_(other.size_)
    {
      if (!data_.empty())
      {
        auto it = data_.before_begin();
        auto next = data_.begin();
        while (next != data_.end())
        {
          it = next;
          ++next;
        }
        tail_ = it;
      }
      other.size_ = 0;
      other.tail_ = other.data_.before_begin();
    }

    Queue< T >& operator=(const Queue< T >& other)
    {
      if (this != std::addressof(other))
      {
        Queue tmp(other);
        swap(tmp);
      }
      return *this;
    }

    Queue< T >& operator=(Queue< T >&& other) noexcept
    {
      if (this != std::addressof(other))
      {
        data_ = static_cast< List< T >&& >(other.data_);
        size_ = other.size_;
        tail_ = data_.before_begin();
        if (!data_.empty())
        {
          auto it = data_.before_begin();
          auto next = data_.begin();
          while (next != data_.end())
          {
            it = next;
            ++next;
          }
          tail_ = it;
        }
        other.size_ = 0;
        other.tail_ = other.data_.before_begin();
      }
      return *this;
    }

    ~Queue() = default;

    void swap(Queue< T >& other) noexcept
    {
      data_.swap(other.data_);
      auto tmpTail = tail_;
      tail_ = other.tail_;
      other.tail_ = tmpTail;
      std::size_t tmpSize = size_;
      size_ = other.size_;
      other.size_ = tmpSize;

      if (data_.empty())
      {
        tail_ = data_.before_begin();
      }
      if (other.data_.empty())
      {
        other.tail_ = other.data_.before_begin();
      }
    }

    void push(const T& value)
    {
      tail_ = data_.insert_after(tail_, value);
      ++size_;
    }

    void push(T&& value)
    {
      tail_ = data_.insert_after(tail_, static_cast< T&& >(value));
      ++size_;
    }

    T drop()
    {
      if (empty())
      {
        throw std::logic_error("queue is empty");
      }
      T value = static_cast< T&& >(data_.front());
      data_.pop_front();
      --size_;
      if (size_ == 0)
      {
        tail_ = data_.before_begin();
      }
      return value;
    }

    T& front()
    {
      if (empty())
      {
        throw std::logic_error("queue is empty");
      }
      return data_.front();
    }

    const T& front() const
    {
      if (empty())
      {
        throw std::logic_error("queue is empty");
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
    typename List< T >::iterator tail_;
    std::size_t size_;
  };
}

#endif
