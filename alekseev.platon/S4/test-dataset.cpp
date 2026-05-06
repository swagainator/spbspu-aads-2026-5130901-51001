#include <boost/test/unit_test.hpp>

#include <sstream>

#include "dataset.hpp"

BOOST_AUTO_TEST_CASE(load_datasets)
{
  {
    std::istringstream input("first 1 name 2 surname\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(alekseev::loadDatasets(input, storage));
    BOOST_TEST(storage.contains("first"));
    BOOST_TEST(storage.get("first").get(1) == "name");
    BOOST_TEST(storage.get("first").get(2) == "surname");
  }
  {
    std::istringstream input("first 1 name\nsecond 2 value\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(alekseev::loadDatasets(input, storage));
    BOOST_TEST(storage.contains("first"));
    BOOST_TEST(storage.contains("second"));
  }
  {
    std::istringstream input("\nfirst 1 name\n  \nsecond\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(alekseev::loadDatasets(input, storage));
    BOOST_TEST(storage.contains("second"));
    BOOST_TEST(storage.get("second").empty());
  }
  {
    std::istringstream input("first key value\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(!alekseev::loadDatasets(input, storage));
  }
  {
    std::istringstream input("first 1\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(!alekseev::loadDatasets(input, storage));
  }
  {
    std::istringstream input("first 1 old 1 new\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(alekseev::loadDatasets(input, storage));
    BOOST_TEST(storage.get("first").get(1) == "new");
  }
  {
    std::istringstream input("limits -2147483648 min 2147483647 max\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(alekseev::loadDatasets(input, storage));
    BOOST_TEST(storage.get("limits").get(-2147483647 - 1) == "min");
    BOOST_TEST(storage.get("limits").get(2147483647) == "max");
  }
  {
    std::istringstream input("first 1 name\nfirst 2 surname\n");
    alekseev::DictionaryStorage storage;
    BOOST_TEST(!alekseev::loadDatasets(input, storage));
  }
}
