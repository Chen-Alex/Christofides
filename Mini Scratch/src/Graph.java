import java.util.*;
public class Graph {
	int number; // number of nodes
	ArrayList<Pair>[] adj;
	
	public Graph (ArrayList<Pair>[] adj) {
		number = adj.length;
		this.adj = adj;
	}
	
	public Graph (ArrayList<Edge> edges) {
		number = 0;
		for (Edge e: edges) {
			number = Math.max(number, e.n1+1);
			number = Math.max(number, e.n2+1);
		}
		adj = new ArrayList[number];
		for(int i=0; i < number; i++) {
			adj[i] = new ArrayList<Pair>();
		}
		for(Edge e:edges) {
			adj[e.n1].add(new Pair(e.n2,e.w));
			adj[e.n2].add(new Pair(e.n1,e.w));
		}
	}

	public void removeEdge(int node, int index) {
		if(node >= adj.length || index >=adj[node].size())
			return;
		int v1 = node;
		int v2 = adj[node].get(index).a;
		double w2 = adj[node].get(index).b;
		
		// remove the edge twice in both direction
		adj[node].remove(index);
		adj[v2].remove(adj[v2].indexOf(new Pair(v1,w2)));
		
	}

	public void dump() {
		for(int i=0; i<adj.length; i++) {
			System.out.println( "node " +i + " adjacent to:");
			for(Pair p: adj[i]) {
				System.out.println(p.a +" weight: "+ p.b);
			}
		}
	}
	
//	@Override
//	public String toString() {
////		return "Graph [number=" + number + ", adj=" + Arrays.toString(adj) + "]";
//		String s ="";
//		for (ArrayList<Pair> p: adj) {
//			s+
//		}
//		return s;
//	}
}
