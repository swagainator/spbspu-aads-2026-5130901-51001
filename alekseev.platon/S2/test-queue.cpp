#include <boost/test/unit_test.hpp>

#include <stdexcept>

#include <queue.hpp>

BOOST_AUTO_TEST_CASE(fifo_order)
{
  alekseev::Queue< int > q;
  q.push(1);
  q.push(2);
  q.push(3);
  BOOST_TEST(q.drop() == 1);
  BOOST_TEST(q.drop() == 2);
  BOOST_TEST(q.drop() == 3);
  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(front_does_not_remove)
{
  alekseev::Queue< int > q;
  q.push(10);
  q.push(20);
  BOOST_TEST(q.front() == 10);
  BOOST_TEST(q.size() == 2);
  BOOST_TEST(q.drop() == 10);
  BOOST_TEST(q.drop() == 20);
}

BOOST_AUTO_TEST_CASE(queue_empty_exceptions)
{
  alekseev::Queue< int > q;
  BOOST_CHECK_THROW(q.drop(), std::logic_error);
  BOOST_CHECK_THROW(q.front(), std::logic_error);
  const alekseev::Queue< int >& cq = q;
  BOOST_CHECK_THROW(cq.front(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(tail_reset_after_empty)
{
  alekseev::Queue< int > q;
  q.push(1);
  BOOST_TEST(q.drop() == 1);
  BOOST_TEST(q.empty());

  q.push(2);
  q.push(3);
  BOOST_TEST(q.drop() == 2);
  BOOST_TEST(q.drop() == 3);
  BOOST_TEST(q.empty());
}

BOOST_AUTO_TEST_CASE(copy_semantics)
{
  alekseev::Queue< int > q;
  q.push(1);
  q.push(2);

  alekseev::Queue< int > copy(q);
  BOOST_TEST(copy.drop() == 1);
  BOOST_TEST(copy.drop() == 2);

  alekseev::Queue< int > assigned;
  assigned.push(9);
  assigned = q;
  BOOST_TEST(assigned.drop() == 1);
  BOOST_TEST(assigned.drop() == 2);

  BOOST_TEST(q.drop() == 1);
  BOOST_TEST(q.drop() == 2);
}

BOOST_AUTO_TEST_CASE(queue_move_semantics_source_valid)
{
  alekseev::Queue< int > q;
  q.push(5);
  q.push(6);

  alekseev::Queue< int > moved(static_cast< alekseev::Queue< int >&& >(q));
  BOOST_TEST(moved.drop() == 5);
  BOOST_TEST(moved.drop() == 6);

  BOOST_TEST(q.empty());
  BOOST_TEST(q.size() == 0);
}
