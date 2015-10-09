/* COPYRIGHT (c) 2015 Sam Westrick, Laxman Dhulipala, Umut Acar,
 * Arthur Chargueraud, and Michael Rainey.
 * All rights reserved.
 *
 * \file pwsa.cpp
 *
 */

#include "benchmark.hpp"
#include "treap-frontier.hpp"
#include "hash.hpp"
// include "weighted-graph.hpp"

// static inline void pmemset(char * ptr, int value, size_t num) {
//   const size_t cutoff = 100000;
//   if (num <= cutoff) {
//     memset(ptr, value, num);
//   } else {
//     long m = num/2;
//     sched::native::fork2([&] {
//       pmemset(ptr, value, m);
//     }, [&] {
//       pmemset(ptr+m, value, num-m);
//     });
//   }
// }

// template <class Number, class Size>
// void fill_array_par(std::atomic<Number>* array, Size sz, Number val) {
//   pmemset((char*)array, val, sz*sizeof(Number));
// }

template <class FRONTIER, class HEURISTIC, class VERTEX>
std::atomic<long*> pwsa(const graph<VERTEX>& graph, const HEURISTIC& heuristic,
                        const vertex& source, const vertex& destination,
                        int split_cutoff, int poll_cutoff) {
  nat N = graph.number_vertices();
  std::atomic<long>* finalized = pasl::data::mynew_array<std::atomic<long>>(N);
  fill_array_par(finalized, N, -1l);

  FRONTIER initF();
  initF.insert(heuristic(source), graph.make_vertex_package(source, false, 0));

  pasl::data::perworker::array<int> work_since_split;
  work_since_split.init(0);

  auto size = [&] (FRONTIER& frontier) {
    auto sz = frontier.total_weight();
    if (sz == 0) {
      work_since_split.mine() = 0;
      return 0; // no work left
    }
    if (sz > split_cutoff || (work_since_split.mine() > split_cutoff && sz > 1))
      return 2; // split
    else
      return 1; // don't split
  };

  auto fork = [&] (FRONTIER& src, FRONTIER& dst) {
    src.split_at(src.total_weight() / 2, dst);
    work_since_split.mine() = 0;
  };

  auto set_in_env = [&] (FRONTIER& f) {
    ; // nothing to do
  };

  auto do_work = [&] (FRONTIER& frontier) {
    int work_this_round = 0;
    while (work_this_round < poll_cutoff && frontier.total_weight() > 0) {
      auto pair = frontier.delete_min();
      VertexPackage vpack = pair.second;
      if (vpack.weight() + work_this_round > poll_cutoff) {
        VertexPackage other();
        vpack.split_at(poll_cutoff - work_this_round, other);
        frontier.insert(heuristic(other.vertex()) + other.distance_to(), other);
      }
      long orig = -1l;
      finalized[vpack.vertex()].compare_exchange_strong(orig, vpack.distance_to());
      long actual_distance = finalized[vpack.vertex()].load(std::memory_order_relaxed);
      graph.apply_to_each_out_neighbor_in(vpack, [&] (const vertex& u, long edge_weight) {
        frontier.insert(heuristic(u) + actual_distance + edge_weight, 
      });
    }

  };

}

// class X {
// public:
//   long x;
//
//   X(long x) : x(x) { }
//
//   long hash() const { return hash_signed(x); }
//
//   inline bool operator < (const X& b) const { return x < b.x; }
//   inline bool operator <= (const X& b) const { return x <= b.x; }
//   inline bool operator == (const X& b) const { return x == b.x; }
// };
//
// std::ostream& operator << (std::ostream& out, const X& a) {
//   return out << a.x;
// }
//
// class Y {
// public:
//   long y;
//
//   Y(long y) : y(y) { }
//
//   long weight() const { return y; }
//
//   void split_at(long w, Y& dst) {
//     dst.y = y - w;
//     y = w;
//   }
// };
//
// std::ostream& operator << (std::ostream& out, const Y& a) {
//   return out << a.y;
// }

int main(int argc, char** argv) {
  long n;

  auto init = [&] {
    n = (long)pasl::util::cmdline::parse_or_default_long("n", 24);
  };

  auto run = [&] (bool sequential) {
    std::cout << n << std::endl;

    auto T = Treap<X,Y>();
// HOWTOGRAPH
//    bool isSym = false;
//    char const* fname = "test2.txt"; (some file using WeightedAdjacencySeq format)
//    graph<asymmetricVertex> g = readGraphFromFile<asymmetricVertex>(fname, isSym);  
//
//    std::cout << "outW = " << g.V[1].getInWeight(1) << std::endl;

    auto T = Treap<long,Range<int,int>>();
    for (int i = 0; i < n; i++) {
      long x = rand() % 1000;
      long y = rand() % 15;
      T.insert(X(x), Y(y));
    }

    T.check();

    auto M = Treap<X,Y>();
    long w = T.total_weight();
    T.split_at(T.total_weight() / 2, M);
    assert(T.total_weight() == w / 2);
    assert(M.total_weight() == w - (w/2));
    T.check();
    M.check();
  };

  auto output = [&] {
    ;
  };

  auto destroy = [&] {
    ;
  };

  pasl::sched::launch(argc, argv, init, run, output, destroy);
  return 0;
}