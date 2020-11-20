//
//  ReusableGraph.cpp
//  toricdecoder
//
//  Created by Jon San Miguel on 11/19/20.
//

#include "ReusableGraph.hpp"
#include "boost/graph/graph_traits.hpp"

template<class vtype, class etype>
class ReusableGraph {
    
};

namespace boost {
  template <class vtype, class etype>
  struct graph_traits< ReusableGraph<vtype,etype> > {
    typedef int vertex_descriptor;
    typedef std::pair<int, int> edge_descriptor;

    // iterator typedefs...

    typedef directed_tag directed_category;
    typedef allow_parallel_edge_tag edge_parallel_category;
    typedef int vertices_size_type;
    typedef int edges_size_type;
  };

    template <class vtype, class etype>
    typename graph_traits< ReusableGraph<vtype,etype> >::vertex_descriptor
    source(
      typename graph_traits< ReusableGraph<vtype,etype> >::edge_descriptor e,
      const ReusableGraph<vtype,etype>& g)
    {
      return e.first;
    }

    template <class vtype, class etype>
    typename graph_traits< ReusableGraph<vtype,etype> >::vertex_descriptor
    target(
      typename graph_traits< ReusableGraph<vtype,etype> >::edge_descriptor e,
      const ReusableGraph<vtype,etype>& g)
    {
      return e.second;
    }
} // namespace boost
