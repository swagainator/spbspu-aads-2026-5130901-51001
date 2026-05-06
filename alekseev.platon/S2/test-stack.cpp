#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <utility>

#include <stack.hpp>

BOOST_AUTO_TEST_CASE(lifo_order)
{
  alekseev::Stack< int > st;
  st.push(1);
  st.push(2);
  st.push(3);
  BOOST_TEST(st.drop() == 3);
  BOOST_TEST(st.drop() == 2);
  BOOST_TEST(st.drop() == 1);
  BOOST_TEST(st.empty());
}

BOOST_AUTO_TEST_CASE(top_does_not_remove)
{
  alekseev::Stack< int > st;
  st.push(10);
  st.push(20);
  BOOST_TEST(st.top() == 20);
  BOOST_TEST(st.size() == 2);
  BOOST_TEST(st.drop() == 20);
  BOOST_TEST(st.drop() == 10);
}

BOOST_AUTO_TEST_CASE(stack_empty_exceptions)
{
  alekseev::Stack< int > st;
  BOOST_CHECK_THROW(st.drop(), std::logic_error);
  BOOST_CHECK_THROW(st.top(), std::logic_error);
  const alekseev::Stack< int >& cst = st;
  BOOST_CHECK_THROW(cst.top(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(copy_semantics_preserve_order)
{
  alekseev::Stack< int > st;
  st.push(1);
  st.push(2);
  st.push(3);

  alekseev::Stack< int > copy(st);
  BOOST_TEST(copy.drop() == 3);
  BOOST_TEST(copy.drop() == 2);
  BOOST_TEST(copy.drop() == 1);

  alekseev::Stack< int > assigned;
  assigned.push(9);
  assigned = st;
  BOOST_TEST(assigned.drop() == 3);
  BOOST_TEST(assigned.drop() == 2);
  BOOST_TEST(assigned.drop() == 1);

  BOOST_TEST(st.drop() == 3);
  BOOST_TEST(st.drop() == 2);
  BOOST_TEST(st.drop() == 1);
}

BOOST_AUTO_TEST_CASE(stack_move_semantics_source_valid)
{
  alekseev::Stack< int > st;
  st.push(1);
  st.push(2);

  alekseev::Stack< int > moved(static_cast< alekseev::Stack< int >&& >(st));
  BOOST_TEST(moved.drop() == 2);
  BOOST_TEST(moved.drop() == 1);

  BOOST_TEST(st.empty());
  BOOST_TEST(st.size() == 0);
}

BOOST_AUTO_TEST_CASE(push_after_drops)
{
  alekseev::Stack< int > st;
  st.push(1);
  st.push(2);
  BOOST_TEST(st.drop() == 2);
  BOOST_TEST(st.drop() == 1);

  st.push(3);
  st.push(4);
  BOOST_TEST(st.drop() == 4);
  BOOST_TEST(st.drop() == 3);
  BOOST_TEST(st.empty());
}
