#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <string>

#include <bstree.hpp>

namespace
{
  template< class Tree >
  std::string keys(const Tree& tree)
  {
    std::string result;
    for (typename Tree::const_iterator it = tree.cbegin(); it != tree.cend(); ++it)
    {
      result += static_cast< char >('0' + it->first);
    }
    return result;
  }
}

BOOST_AUTO_TEST_CASE(empty_tree)
{
  const alekseev::BSTree< int, std::string > tree;
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(static_cast< bool >(tree.begin() == tree.end()));
  BOOST_TEST(tree.height() == 0);
}

BOOST_AUTO_TEST_CASE(insert_search_and_iterators)
{
  alekseev::BSTree< int, std::string > tree;
  tree.push(2, "two");
  BOOST_TEST(!tree.empty());
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(2) == "two");
  tree.push(1, "one");
  tree.push(3, "three");
  BOOST_TEST(keys(tree) == "123");
  tree.push(2, "changed");
  BOOST_TEST(tree.size() == 3);
  BOOST_TEST(tree.get(2) == "changed");
  BOOST_TEST(tree.contains(1));
  BOOST_TEST(!tree.contains(4));
  BOOST_CHECK_THROW(tree.get(4), std::out_of_range);

  alekseev::BSTree< int, std::string >::iterator it = tree.begin();
  BOOST_TEST(it->first == 1);
  BOOST_TEST((++it)->first == 2);
  BOOST_TEST((it++)->first == 2);
  BOOST_TEST(it->first == 3);
  it = tree.end();
  --it;
  BOOST_TEST(it->first == 3);

  const alekseev::BSTree< int, std::string >& constTree = tree;
  alekseev::BSTree< int, std::string >::const_iterator cit = constTree.cbegin();
  BOOST_TEST(cit->first == 1);
  BOOST_TEST(cit->second == "one");
}

BOOST_AUTO_TEST_CASE(copy_move_and_clear)
{
  alekseev::BSTree< int, std::string > tree;
  tree.push(2, "two");
  tree.push(1, "one");
  tree.push(3, "three");

  alekseev::BSTree< int, std::string > copy(tree);
  copy.push(4, "four");
  BOOST_TEST(keys(tree) == "123");
  BOOST_TEST(keys(copy) == "1234");

  alekseev::BSTree< int, std::string > assigned;
  assigned = tree;
  assigned.drop(1);
  BOOST_TEST(keys(tree) == "123");
  BOOST_TEST(keys(assigned) == "23");

  alekseev::BSTree< int, std::string > moved(std::move(copy));
  BOOST_TEST(copy.empty());
  BOOST_TEST(static_cast< bool >(copy.begin() == copy.end()));
  BOOST_TEST(keys(moved) == "1234");

  alekseev::BSTree< int, std::string > moveAssigned;
  moveAssigned = std::move(moved);
  BOOST_TEST(moved.empty());
  BOOST_TEST(static_cast< bool >(moved.begin() == moved.end()));
  BOOST_TEST(keys(moveAssigned) == "1234");

  moveAssigned.clear();
  BOOST_TEST(moveAssigned.empty());
  BOOST_TEST(moveAssigned.size() == 0);

  alekseev::BSTree< int, std::string > left;
  left.push(1, "one");
  alekseev::BSTree< int, std::string > right;
  right.push(2, "two");
  right.push(3, "three");
  left.swap(right);
  BOOST_TEST(keys(left) == "23");
  BOOST_TEST(keys(right) == "1");
}

BOOST_AUTO_TEST_CASE(drop_and_height)
{
  alekseev::BSTree< int, std::string > tree;
  tree.push(4, "four");
  tree.push(2, "two");
  tree.push(6, "six");
  tree.push(1, "one");
  tree.push(3, "three");
  tree.push(5, "five");
  tree.push(7, "seven");
  BOOST_TEST(tree.height() == 3);
  BOOST_TEST(tree.drop(1) == "one");
  BOOST_TEST(keys(tree) == "234567");
  tree.push(1, "one");
  tree.drop(2);
  BOOST_TEST(keys(tree) == "134567");
  tree.drop(6);
  BOOST_TEST(keys(tree) == "13457");
  tree.drop(7);
  BOOST_TEST(keys(tree) == "1345");
  tree.drop(5);
  BOOST_TEST(keys(tree) == "134");
  tree.drop(4);
  BOOST_TEST(keys(tree) == "13");

  alekseev::BSTree< int, std::string > one;
  one.push(10, "ten");
  BOOST_TEST(one.height() == 1);
  one.push(9, "nine");
  one.push(8, "eight");
  BOOST_TEST(one.height(one.begin()) == 1);
  BOOST_TEST(one.height(one.end()) == 0);
}

BOOST_AUTO_TEST_CASE(rotations)
{
  alekseev::BSTree< int, std::string > leftTree;
  leftTree.push(2, "two");
  leftTree.push(1, "one");
  leftTree.push(4, "four");
  leftTree.push(3, "three");
  alekseev::BSTree< int, std::string >::iterator stable = leftTree.begin();
  ++stable;
  alekseev::BSTree< int, std::string >::iterator raised = leftTree.rotateLeft(--leftTree.end());
  BOOST_TEST(raised->first == 4);
  BOOST_TEST(keys(leftTree) == "1234");
  BOOST_TEST(stable->first == 2);
  BOOST_TEST(stable->second == "two");

  alekseev::BSTree< int, std::string > rightTree;
  rightTree.push(3, "three");
  rightTree.push(1, "one");
  rightTree.push(4, "four");
  rightTree.push(2, "two");
  raised = rightTree.rotateRight(rightTree.begin());
  BOOST_TEST(raised->first == 1);
  BOOST_TEST(keys(rightTree) == "1234");

  alekseev::BSTree< int, std::string > largeLeft;
  largeLeft.push(1, "one");
  largeLeft.push(4, "four");
  largeLeft.push(2, "two");
  largeLeft.push(3, "three");
  alekseev::BSTree< int, std::string >::iterator inner = largeLeft.begin();
  ++inner;
  raised = largeLeft.rotateLargeLeft(inner);
  BOOST_TEST(raised->first == 2);
  BOOST_TEST(keys(largeLeft) == "1234");

  alekseev::BSTree< int, std::string > largeRight;
  largeRight.push(4, "four");
  largeRight.push(1, "one");
  largeRight.push(3, "three");
  largeRight.push(2, "two");
  inner = largeRight.begin();
  ++inner;
  ++inner;
  raised = largeRight.rotateLargeRight(inner);
  BOOST_TEST(raised->first == 3);
  BOOST_TEST(keys(largeRight) == "1234");

  alekseev::BSTree< int, std::string > rootRotate;
  rootRotate.push(1, "one");
  rootRotate.push(2, "two");
  rootRotate.rotateLeft(--rootRotate.end());
  BOOST_TEST(keys(rootRotate) == "12");
  BOOST_TEST(rootRotate.height() == 2);
  BOOST_CHECK_THROW(rootRotate.rotateLeft(rootRotate.begin()), std::logic_error);
}
