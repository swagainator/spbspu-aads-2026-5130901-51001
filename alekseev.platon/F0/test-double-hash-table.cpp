#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

#include "double_hash_table.hpp"

namespace
{
  struct IntHash
  {
    std::size_t operator()(int value) const noexcept
    {
      return static_cast< std::size_t >(value);
    }
  };

  struct ConstantHash
  {
    std::size_t operator()(int) const noexcept
    {
      return 3;
    }
  };

  struct IntEqual
  {
    bool operator()(int lhs, int rhs) const noexcept
    {
      return lhs == rhs;
    }
  };

  using IntTable = alekseev::DoubleHashTable< int, std::string, IntHash, IntEqual >;
  using CollisionTable =
      alekseev::DoubleHashTable< int, std::string, ConstantHash, IntEqual >;
}

BOOST_AUTO_TEST_CASE(double_hash_table_empty)
{
  const IntTable table(3);

  BOOST_CHECK(table.empty());
  BOOST_CHECK_EQUAL(table.size(), 0);
  BOOST_CHECK(table.capacity() >= 11);
  BOOST_CHECK(table.begin() == table.end());
  BOOST_CHECK(!table.has(1));
}

BOOST_AUTO_TEST_CASE(double_hash_table_add_find_and_duplicate)
{
  IntTable table(11);
  table.add(1, "one");
  table.add(12, "twelve");

  BOOST_CHECK(!table.empty());
  BOOST_CHECK_EQUAL(table.size(), 2);
  BOOST_CHECK(table.has(1));
  BOOST_CHECK(table.has(12));
  BOOST_CHECK_EQUAL(table.at(1), "one");
  BOOST_CHECK_EQUAL(table.at(12), "twelve");
  BOOST_CHECK_THROW(table.add(1, "changed"), std::logic_error);
  BOOST_CHECK_THROW(table.at(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(double_hash_table_collision_probe)
{
  CollisionTable table(11);
  for (int i = 0; i < 8; ++i)
  {
    table.add(i, std::to_string(i));
  }

  for (int i = 0; i < 8; ++i)
  {
    BOOST_CHECK(table.has(i));
    BOOST_CHECK_EQUAL(table.at(i), std::to_string(i));
  }
}

BOOST_AUTO_TEST_CASE(double_hash_table_tombstone_and_reinsert)
{
  CollisionTable table(11);
  table.add(1, "one");
  table.add(2, "two");
  table.add(3, "three");

  BOOST_CHECK_EQUAL(table.drop(2), "two");
  BOOST_CHECK(!table.has(2));
  BOOST_CHECK(table.has(1));
  BOOST_CHECK(table.has(3));
  BOOST_CHECK_EQUAL(table.tombstoneCount(), 1);

  table.add(4, "four");
  BOOST_CHECK(table.has(4));
  BOOST_CHECK_EQUAL(table.tombstoneCount(), 0);
  BOOST_CHECK_THROW(table.drop(2), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(double_hash_table_rehash_preserves_values)
{
  IntTable table(11);
  for (int i = 0; i < 8; ++i)
  {
    table.add(i, std::to_string(i));
  }
  BOOST_CHECK_EQUAL(table.drop(3), "3");
  BOOST_CHECK_EQUAL(table.drop(5), "5");
  BOOST_CHECK_EQUAL(table.tombstoneCount(), 2);

  table.rehash(31);
  BOOST_CHECK(table.capacity() >= 31);
  BOOST_CHECK_EQUAL(table.size(), 6);
  BOOST_CHECK_EQUAL(table.tombstoneCount(), 0);
  for (int i = 0; i < 8; ++i)
  {
    if (i != 3 && i != 5)
    {
      BOOST_CHECK(table.has(i));
      BOOST_CHECK_EQUAL(table.at(i), std::to_string(i));
    }
  }
}

BOOST_AUTO_TEST_CASE(double_hash_table_overflow)
{
  CollisionTable table(3);
  for (int i = 0; i < 11; ++i)
  {
    table.add(i, std::to_string(i));
  }

  BOOST_CHECK_THROW(table.add(11, "11"), std::overflow_error);
}

BOOST_AUTO_TEST_CASE(double_hash_table_copy_move_and_iterators)
{
  IntTable table(11);
  for (int i = 0; i < 5; ++i)
  {
    table.add(i, std::to_string(i));
  }
  BOOST_CHECK_EQUAL(table.drop(2), "2");

  IntTable copy(table);
  copy.add(10, "10");
  BOOST_CHECK(!table.has(10));
  BOOST_CHECK(copy.has(10));

  IntTable assigned;
  assigned = table;
  BOOST_CHECK(assigned.has(1));
  BOOST_CHECK(!assigned.has(2));

  IntTable moved(std::move(copy));
  BOOST_CHECK(copy.empty());
  BOOST_CHECK(moved.has(10));

  IntTable moveAssigned;
  moveAssigned = std::move(moved);
  BOOST_CHECK(moved.empty());
  BOOST_CHECK(moveAssigned.has(10));

  std::size_t count = 0;
  for (IntTable::const_iterator it = moveAssigned.cbegin(); it != moveAssigned.cend(); ++it)
  {
    BOOST_CHECK(moveAssigned.has(it->key()));
    ++count;
  }
  BOOST_CHECK_EQUAL(count, moveAssigned.size());
}
