
// Copyright Ricardo Calheiros de Miranda Cosme 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <coruja/container/transform.hpp>
#include <coruja/container/vector.hpp>
#include <coruja/object/object.hpp>
#include <iostream>
#include <algorithm>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <gtk/gtk.h>
#include "timer.hpp"

struct marker {
    coruja::object<std::string> name;
    coruja::object<float> top, base;
};

struct well {
    std::string name;
    coruja::vector<marker> markers;
};

inline void warmup(coruja::vector<well>& wells)
{
    wells.reserve(1000);
    for(std::size_t i(0); i < 1000; ++i)
        wells.push_back(well{std::to_string(i)});
    wells.clear();
}

std::pair<GtkWidget*, GtkWidget*> setup(GtkTreeStore* store)
{
    GtkWidget* window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *tree;
    tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
    
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("Name",
                                                       renderer,
                                                       "text", 0,
                                                       NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);
    gtk_container_add (GTK_CONTAINER (window), tree);
    gtk_widget_show(tree);
    return {window, tree};
}

inline void free(GtkTreeStore* store,
                 std::pair<GtkWidget*, GtkWidget*> widgets)
{
    g_object_unref(store);
    gtk_widget_destroy(widgets.second);
    gtk_widget_destroy(widgets.first);
}

void bench(GtkTreeStore* store, coruja::vector<well>& wells)
{
    auto il{50, 200, 500, 1000, 1500, 2000, 3000, 4000, 5000, 10000};
    for(auto e : il)
    {
        std::vector<int> m;
        std::size_t num = e < 1500 ? 250 : 10;
        for(std::size_t j(0); j < num; ++j)
            {
                wells.reserve(e);
                {
                    timer<std::chrono::microseconds> t;
                    for(std::size_t i(0); i < e; ++i)
                        wells.push_back(well{std::to_string(i)});
                    m.emplace_back(t.stop());
                }
                wells.clear();
                gtk_tree_store_clear(store);
            }
        using namespace boost;
        using namespace boost::accumulators;
        accumulator_set<double, stats<tag::variance> > acc;
        acc = std::for_each(m.begin(), m.end(), acc);
        std::cout << "t " << mean(acc)
                  << " [" << sqrt(variance(acc)) << "]" << std::endl;
    }
}

void run(GtkTreeStore* store, coruja::vector<well>& wells)
{
    auto widgets = setup(store);

    warmup(wells);
    bench(store, wells);

    free(store, widgets);
}

void coruja_solution()
{
    coruja::vector<well> wells;
    
    using namespace coruja;
    using namespace std;
    
    auto rng = 
        transform(wells, 
         [](well& w)
         { return make_pair(std::ref(w.name), transform
                            (w.markers, [](marker& m) -> coruja::object<std::string>&
                             { return m.name; })); });

    using Rng = decltype(rng);
    GtkTreeStore *store = gtk_tree_store_new (1, G_TYPE_STRING);
    rng.for_each
        ([store](Rng& wells, ranges::range_iterator_t<Rng> it)
         {
             auto well_pos = ranges::distance(begin(wells),it);
             GtkTreeIter iter;
             gtk_tree_store_insert (store, &iter, NULL, well_pos);
             gtk_tree_store_set (store, &iter, 0, (*it).first.c_str(), -1);
             using Rng2 = decltype((*it).second);
             (*it).second.for_each
                 ([store, iter](Rng2& markers,
                                ranges::range_iterator_t<Rng2> it)
                  {
                      auto position = ranges::distance(begin(markers), it);
                      GtkTreeIter child;
                      gtk_tree_store_insert (store, &child, const_cast<GtkTreeIter*>(&iter), position);
                      gtk_tree_store_set (store, &child, 0, (*it).get().c_str(), -1);
                  });
         });
    run(store, wells);
}

void adhoc_solution()
{
    coruja::vector<well> wells;
    
    using namespace coruja;
    using namespace std;
    
    GtkTreeStore *store = gtk_tree_store_new (1, G_TYPE_STRING);
    using Rng = coruja::vector<well>;
    wells.for_each
        ([store](Rng& wells, ranges::range_iterator_t<Rng> it)
         {
             auto well_pos = ranges::distance(begin(wells),it);
             GtkTreeIter iter;
             gtk_tree_store_insert (store, &iter, NULL, well_pos);
             gtk_tree_store_set (store, &iter, 0, (*it).name.c_str(), -1);
             using Rng2 = decltype((*it).markers);
             (*it).markers.for_each
                 ([store, iter](Rng2& markers,
                                ranges::range_iterator_t<Rng2> it)
                  {
                      auto position = ranges::distance(begin(markers), it);
                      GtkTreeIter child;
                      gtk_tree_store_insert (store, &child, const_cast<GtkTreeIter*>(&iter), position);
                      gtk_tree_store_set (store, &child, 0, (*it).name.get().c_str(), -1);
                  });
         });
    run(store, wells);
}

int main(int argc, char** argv)
{
    gtk_init(&argc, &argv);

    std::cout << "transform" << std::endl << std::endl;
    coruja_solution();
    std::cout << std::endl << "adhoc" << std::endl;
    adhoc_solution();
}
