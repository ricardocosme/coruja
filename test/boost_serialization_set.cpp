#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/coruja_set.hpp>
#include <boost/serialization/string.hpp>

template<typename ocontainer, typename expected_t>
void check_equal(const ocontainer& c, const expected_t& expected)
{
    assert(c.size() == expected.size());
    for(auto& e : c)
        assert(expected.find(e) != expected.end());
}

#include <fstream>
#include <string>

int main()
{
    const std::string file("/tmp/coruja_set_serialization");
    {
        coruja::set<std::string> obj{"abc", "def", "ghi"};
        std::ofstream out(file);
        boost::archive::text_oarchive oa(out);
        oa << obj;
    }
        
    {
        coruja::set<std::string> obj;
        std::ifstream in(file);
        boost::archive::text_iarchive ia(in);
        ia >> obj;
        std::set<std::string> expected{"abc", "def", "ghi"};
        check_equal(obj, expected);
    }
    
}
