#include <coruja/object/object.hpp>
#include <coruja/support/signal.hpp>
#include "timer.hpp"
#include <cassert>
#include <string>
#include <vector>
using namespace coruja;

using sig_t = signal<void(std::string)>;

int main()
{
    {
        sig_t sig;
        sig("abc");
    }
    
    {
        sig_t sig;
        bool called(false);
        bool snd_called(false);
        sig.connect
            ([&called](std::string s)
             {
                 assert(s == "abc");
                 called = true;
             });
        auto conn = sig.connect
            ([&snd_called](std::string s)
             {
                 assert(s == "abc");
                 snd_called = true;
             });
        sig("abc");
        assert(called);
        assert(snd_called);

        called = false;
        snd_called = false;
        conn.disconnect();
        sig("abc");
        assert(called);
        assert(!snd_called);
    }
    
    {
        sig_t sig;
        bool called(false);
        {
            scoped_connection<sig_t::connection_t> conn
                = sig.connect
                ([&called](std::string s)
                 {
                     assert(s == "abc");
                     called = true;
                 });
            sig("abc");
            assert(called);
            called = false;
        }
        sig("another");
        assert(!called);
    }

    {
        object<std::string> o;
        bool called(false);
        o.after_change([&called](std::string)
                       { called = true; });
        o = "abc";
        assert(called);
    }

    {
        object<std::string> o;
        bool called(false);
        auto conn = o.after_change([&called](std::string)
                                   { called = true; });
        assert(!conn.blocked());
        {
            scoped_blocked_connection<decltype(conn)> scb(conn);
            o = "abc";
            assert(!called);
        }
        o = "abc";
        assert(called);
    }

    {
        bool called{false};
        sig_t sig;
        {
            scoped_any_connection sc =
                sig.connect([&called](std::string){called = true;});
            {
                scoped_blocked_any_connection sbc(sc.release());
                sig("abc");
                assert(!called);
                sc = sbc.connection();
            }
            sig("abc");
            assert(called);
            called = false;
        }
        sig("abc");
        assert(!called);
    }

    //connection equality/inequality
    {
        sig_t::connection_t conn1;
        sig_t::connection_t conn2;
        assert(conn1 == conn2);
        sig_t sig;
        conn1 = sig.connect([](std::string){});
        assert(conn1 != conn2);
        conn2 = conn1;
        assert(conn1 == conn2);
    }
    
    {
        using obj_t = object<std::string>;
        obj_t o;
        bool called(false);
        scoped_any_connection conn = o.after_change
            ([&called](std::string){called = true;});
        auto o2 = std::move(o);
        conn.~scoped_connection();
        o2 = "abc";
        assert(!called);
    }

    {
        using obj_t = object<std::string>;
        bool called{false};
        obj_t o;
        any_connection any_conn = o.after_change([&called](std::string){called = true;});
        o = 5;
        assert(called);
        any_conn.disconnect();
        called = false;
        o = 10;
        assert(!called);
    }

    {
        using obj_t = object<std::string>;
        bool called{false};
        obj_t o;
        {
            scoped_any_connection scoped_conn = 
                o.after_change([&called](std::string){called = true;});
            o = 5;
            assert(called);
        }
        called = false;
        o = 10;
        assert(!called);
    }    

    //equality and inequality
    {
        any_connection c1_empty;
        any_connection c1{sig_t::connection_t()};
        any_connection c2_empty;
        assert(c1_empty == c2_empty);
        assert(!(c1_empty == c1));
        assert(!(c1 == c1_empty));
        assert(c1 == c1);
    }

    //operator<
    {
        any_connection lhs, rhs;
        lhs < rhs;
    }

    {
        any_connection c1;
        any_connection c2(c1);
        assert(c2 == c1);
    }
    
    {
        any_connection c1;
        any_connection c2(std::move(c1));
        assert(c2 == c1);
    }
    
    {
        timer<std::chrono::milliseconds> t;
        std::vector<sig_t::connection_t> conns;
        // std::vector<any_connection> conns;
        sig_t sig;
        for(std::size_t i(0); i < 10000000; ++i)
            conns.emplace_back(sig.connect([](std::string){}));
    }
}
