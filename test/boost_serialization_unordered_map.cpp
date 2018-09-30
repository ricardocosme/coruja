#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/coruja_object.hpp>
#include <boost/serialization/coruja_unordered_map.hpp>
#include <boost/serialization/string.hpp>

template<typename ocontainer, typename expected_t>
void check_equal(const ocontainer& c, const expected_t& expected)
{
    assert(c.size() == expected.size());
    for(auto& e : c)
        assert(expected.at(e.first) == e.second);
}

#include <fstream>
#include <string>

int main()
{
    const std::string file("/tmp/coruja_object_serialization");
    {
        coruja::unordered_map<int, std::string> obj
        {{0, "abc"}, {1, "def"}, {2, "ghi"}};
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << obj;
    }
        
    {
        coruja::unordered_map<int, std::string> obj;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> obj;
        std::unordered_map<int, std::string> expected
            {{0, "abc"}, {1, "def"}, {2, "ghi"}};
        check_equal(obj, expected);
    }
    
    {
        coruja::unordered_map<int, coruja::object<std::string>> obj;
        obj.emplace(0, std::string{"abc"});
        obj.emplace(1, std::string{"def"});
        obj.emplace(2, std::string{"ghi"});
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << obj;
    }
    
    {
        coruja::unordered_map<int, coruja::object<std::string>> obj;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> obj;
        coruja::unordered_map<int, coruja::object<std::string>> expected;
        expected.emplace(0, std::string{"abc"});
        expected.emplace(1, std::string{"def"});
        expected.emplace(2, std::string{"ghi"});
        check_equal(obj, expected);
    }
}
