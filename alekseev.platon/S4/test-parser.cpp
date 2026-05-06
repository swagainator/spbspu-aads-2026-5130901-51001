#include <boost/test/unit_test.hpp>

#include <stdexcept>

#include <line_parser.hpp>

BOOST_AUTO_TEST_CASE(line_parser)
{
  alekseev::LineParser words("  print first ");
  BOOST_TEST(words.hasNext());
  BOOST_TEST(words.readWord() == "print");
  BOOST_TEST(words.readWord() == "first");
  words.expectEnd();
  BOOST_TEST(!words.hasNext());

  alekseev::LineParser tabs("\talpha\tbeta");
  BOOST_TEST(tabs.readWord() == "alpha");
  BOOST_TEST(tabs.readWord() == "beta");
  tabs.expectEnd();

  alekseev::LineParser positive("42");
  BOOST_TEST(positive.readInt() == 42);
  positive.expectEnd();

  alekseev::LineParser signedPositive("+42");
  BOOST_TEST(signedPositive.readInt() == 42);

  alekseev::LineParser negative("-17");
  BOOST_TEST(negative.readInt() == -17);

  BOOST_CHECK_THROW(
  []()
  {
    alekseev::LineParser parser("12x");
    parser.readInt();
  }(), std::invalid_argument);
  BOOST_CHECK_THROW(
  []()
  {
    alekseev::LineParser parser("2147483648");
    parser.readInt();
  }(), std::invalid_argument);
  BOOST_CHECK_THROW(
  []()
  {
    alekseev::LineParser parser("-2147483649");
    parser.readInt();
  }(), std::invalid_argument);
  BOOST_CHECK_THROW(
  []()
  {
    alekseev::LineParser parser("");
    parser.readWord();
  }(), std::invalid_argument);
  BOOST_CHECK_THROW(
  []()
  {
    alekseev::LineParser parser("one two");
    parser.readWord();
    parser.expectEnd();
  }(), std::invalid_argument);

  alekseev::LineParser empty("   ");
  BOOST_TEST(!empty.hasNext());
}
