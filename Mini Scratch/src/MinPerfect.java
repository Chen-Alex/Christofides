import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.util.*;
public class MinPerfect {
	public static Graph MST(Graph graph) {
		HashSet<Integer> visited = new HashSet<Integer>();
		PriorityQueue<Edge> edgeQueue = new PriorityQueue<Edge>();
		visited.add(0);
		for(Pair p: graph.adj[0]) {
			Edge e= new Edge(0,p.a,p.b);
			edgeQueue.add(e);
		}
		
		ArrayList<Edge> edges = new ArrayList<Edge>();
		
		while(visited.size() < graph.number) {
			Edge e = edgeQueue.peek();
			while(visited.contains(e.n1) && visited.contains(e.n2)) {
				edgeQueue.poll();
				e = edgeQueue.peek();
			}
			edgeQueue.remove();
			edges.add(e);
			int expand = !visited.contains(e.n1)? e.n1: e.n2;
			for(Pair p: graph.adj[expand]) {
				Edge f =  new Edge(expand, p.a,p.b);
				if(!visited.contains(p.a))
					edgeQueue.add(f);
			}
			visited.add(e.n1);
			visited.add(e.n2);
		}
		return new Graph(edges);
	}
	
	public static ArrayList<Integer> eulerCycle(Graph graph) {
		
		Stack<Integer> fwd = new Stack<Integer>();
		Stack<Integer> bkwd = new Stack<Integer>();
		int start = 0;
		fwd.push(start);
		while(true) {
			fwd.push(graph.adj[start].get(0).a);
			graph.removeEdge(start, 0);
			while (graph.adj[fwd.peek()].size()>0) {
				int k = fwd.peek();
				fwd.push(graph.adj[k].get(0).a);
				graph.removeEdge(k, 0);
			}
			
			while (graph.adj[fwd.peek()].size()== 0) {
				bkwd.push(fwd.peek());
				fwd.pop();
				if (fwd.empty()) {
					ArrayList<Integer> path = new ArrayList<Integer>();
					while(!bkwd.empty()){
						path.add(bkwd.pop());
					}
					return path;
				}		
			}
			start = fwd.peek();
		}
	}
	
	public static void main(String args[]) {
		for(int id =0; id<=99; id++) {
			File file = new File("uniform/out"+id+"g.txt");
			try {
				Scanner sc = new Scanner(file);
				//number of edges
				int m = sc.nextInt();
				ArrayList<Edge> edges = new ArrayList<Edge>();
				for(int i=0; i<m; i++) {
					int v1 = sc.nextInt();
					int v2 = sc.nextInt();
					double w= sc.nextDouble();
					edges.add(new Edge(v1,v2,w));
				}
				Graph G = new Graph(edges);
				Graph mst = MST(G);
	//			mst.dump();
				ArrayList<Integer> O = new ArrayList<Integer>();
				//induced graph
				ArrayList<float[]> induced = new ArrayList<float[]>();
				for(int i=0; i< mst.adj.length; i++) {
					if(mst.adj[i].size()%2 ==1) {
						O.add(i);
	//					for(Pair p : G.adj[i]) {
	//						if(i < p.a) {
	//							induced.add(new float[]{i,p.a, (float)-p.b});
	//						}
	//					}
					}
				}
				for(int i: O) {
					for(Pair p: G.adj[i]) {
						if(i <p.a && O.contains(p.a)) {
							induced.add(new float[]{i,p.a, (float)-p.b});
						}
					}
				}
				float[][] induceG = new float[induced.size()][3];
				for(int i=0; i<induced.size(); i++) {
					induceG[i] = induced.get(i);
				}
				
				// do the blossom
				int[] minPM = new Blossom(induceG,true).maxWeightMatching();
				
				// now add adj to mst
				for(int i=0; i< minPM.length; i++) {
					if(minPM[i]>=0) {
						double w = 0;
						for(Pair p: G.adj[i]) {
	//						System.out.println(p);
	//						System.out.println(minPM[i]);
							if (p.a == minPM[i]) {
								w=p.b;
							}
						}
						mst.adj[i].add(new Pair(minPM[i],w));
					}
				}
	//			mst.dump();
				// now mst is a multigraph
				ArrayList<Integer> euler = eulerCycle(mst);
				ArrayList<Integer> path = new ArrayList<Integer>();
				HashSet<Integer> repeat = new HashSet<Integer>();
				for(Integer i: euler) {
					if(!repeat.contains(i)) {
						repeat.add(i);
						path.add(i);
					}
				}
				// path is the vertices of the actual path
				System.out.println(path);
				int i=0;
				double cost =0;
				while(i<path.size()) {
					int v1 = path.get(i);
					int v2 = path.get((i+1)%path.size());
					i++;
					for(Pair p: G.adj[v1]) {
						if (p.a == v2) {
							cost+= p.b;
						}
					}
				}
				System.out.println("the cost is "+ cost);
				File wfile = new File("output.txt");

		        // if file doesnt exists, then create it
		        if (!wfile.exists()) {
		            wfile.createNewFile();
		        }
		        StringBuilder sb = new StringBuilder();
		        Formatter formatter = new Formatter(sb, Locale.US);
		        String content = formatter.format("%d,%.2f", id,cost).toString();
		        FileWriter fw = new FileWriter(wfile.getAbsoluteFile(),true);
		        BufferedWriter bw = new BufferedWriter(fw);
		        bw.newLine();
		        bw.write(content);
		        bw.close();
			} catch (Exception e) {
		        e.printStackTrace();
		        System.out.println("file not found");
		    }
		}
	}
}
