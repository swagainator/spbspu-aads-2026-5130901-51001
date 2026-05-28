#include <boost/test/unit_test.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

#include "rb_tree.hpp"

namespace
{
  std::string joinKeys(const alekseev::RBTree< int, std::string >& tree)
  {
    std::string result;
    for (alekseev::RBTree< int, std::string >::const_iterator it = tree.cbegin();
        it != tree.cend(); ++it)
    {
      if (!result.empty())
      {
        result += ' ';
      }
      result += std::to_string(it->first);
    }
    return result;
  }

  void fillAscending(alekseev::RBTree< int, std::string >& tree, int count)
  {
    for (int i = 0; i < count; ++i)
    {
      tree.push(i, std::to_string(i));
    }
  }
}

BOOST_AUTO_TEST_CASE(rb_tree_empty)
{
  const alekseev::RBTree< int, std::string > tree;

  BOOST_CHECK(tree.empty());
  BOOST_CHECK_EQUAL(tree.size(), 0);
  BOOST_CHECK_EQUAL(tree.height(), 0);
  BOOST_CHECK(tree.checkInvariants());
  BOOST_CHECK(tree.begin() == tree.end());
}

BOOST_AUTO_TEST_CASE(rb_tree_insert_ascending)
{
  alekseev::RBTree< int, std::string > tree;
  fillAscending(tree, 20);

  BOOST_CHECK_EQUAL(tree.size(), 20);
  BOOST_CHECK_EQUAL(joinKeys(tree), "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19");
  BOOST_CHECK_EQUAL(tree.get(0), "0");
  BOOST_CHECK_EQUAL(tree.get(19), "19");
  BOOST_CHECK(tree.contains(12));
  BOOST_CHECK(!tree.contains(42));
  BOOST_CHECK(tree.height() < 20);
  BOOST_CHECK(tree.checkInvariants());
}

BOOST_AUTO_TEST_CASE(rb_tree_insert_descending_and_mixed)
{
  alekseev::RBTree< int, std::string > descending;
  for (int i = 20; i > 0; --i)
  {
    descending.push(i, std::to_string(i));
  }
  BOOST_CHECK_EQUAL(joinKeys(descending), "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20");
  BOOST_CHECK(descending.height() < 20);
  BOOST_CHECK(descending.checkInvariants());

  alekseev::RBTree< int, std::string > mixed;
  const int values[] = {7, 3, 18, 10, 22, 8, 11, 26, 2, 6, 13};
  const std::size_t count = sizeof(values) / sizeof(values[0]);
  for (std::size_t i = 0; i < count; ++i)
  {
    mixed.push(values[i], std::to_string(values[i]));
  }
  BOOST_CHECK_EQUAL(joinKeys(mixed), "2 3 6 7 8 10 11 13 18 22 26");
  BOOST_CHECK(mixed.checkInvariants());
}

BOOST_AUTO_TEST_CASE(rb_tree_duplicate_updates_value)
{
  alekseev::RBTree< int, std::string > tree;
  tree.push(5, "five");
  tree.push(5, "changed");

  BOOST_CHECK_EQUAL(tree.size(), 1);
  BOOST_CHECK_EQUAL(tree.get(5), "changed");
  BOOST_CHECK(tree.checkInvariants());
}

BOOST_AUTO_TEST_CASE(rb_tree_copy_and_move)
{
  alekseev::RBTree< int, std::string > tree;
  fillAscending(tree, 12);

  alekseev::RBTree< int, std::string > copy(tree);
  copy.push(20, "20");
  BOOST_CHECK_EQUAL(tree.size(), 12);
  BOOST_CHECK_EQUAL(copy.size(), 13);
  BOOST_CHECK(copy.checkInvariants());

  alekseev::RBTree< int, std::string > assigned;
  assigned = tree;
  assigned.push(30, "30");
  BOOST_CHECK(!tree.contains(30));
  BOOST_CHECK(assigned.contains(30));
  BOOST_CHECK(assigned.checkInvariants());

  alekseev::RBTree< int, std::string > moved(std::move(copy));
  BOOST_CHECK(copy.empty());
  BOOST_CHECK(moved.contains(20));
  BOOST_CHECK(moved.checkInvariants());

  alekseev::RBTree< int, std::string > moveAssigned;
  moveAssigned = std::move(moved);
  BOOST_CHECK(moved.empty());
  BOOST_CHECK(moveAssigned.contains(20));
  BOOST_CHECK(moveAssigned.checkInvariants());
}

BOOST_AUTO_TEST_CASE(rb_tree_lookup_missing_key)
{
  alekseev::RBTree< int, std::string > tree;
  tree.push(1, "one");

  BOOST_CHECK_THROW(tree.get(2), std::out_of_range);
}
