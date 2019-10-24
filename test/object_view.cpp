#include <coruja/object/object.hpp>
#include <coruja/object/view/object.hpp>

#include <boost/core/lightweight_test.hpp>

#include <memory>

using namespace coruja;
using namespace std;

int main() {
    //view::object<ObservableObject>::observed() must return the type
    //returned by ObservableObject::observed().
    {
        using object_t = object<unique_ptr<int>>;
        object_t o{unique_ptr<int>{new int{5}}};
        view::object<object_t> vo{o};
        BOOST_TEST(vo.observed().get() == vo.get().get());
        BOOST_TEST(vo.observed().get() != nullptr);
    }
}
