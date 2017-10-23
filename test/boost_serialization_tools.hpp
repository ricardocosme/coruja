
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <string>

template<typename Save, typename Load>
inline void run(Save&& save, Load&& load)
{
    using namespace boost::filesystem;
    auto path = temp_directory_path();
    path /= unique_path();
    auto spath = path.native();
    save(spath);
    load(spath);
    remove(path);
}

template<typename Obj>
inline void save_file(std::string path, const Obj& obj)
{
    std::ofstream out(path);
    boost::archive::text_oarchive oa(out);
    oa << obj;
}

template<typename Obj>
inline void load_file(std::string path, Obj& obj)
{
    std::ifstream in(path);
    boost::archive::text_iarchive ia(in);
    ia >> obj;
}

