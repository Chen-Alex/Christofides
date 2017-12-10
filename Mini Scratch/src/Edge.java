
public class Edge implements Comparable<Edge>{
	int n1;
	int n2;
	double w;
	public Edge(int a,int b, double w) {
		n1 = a;
		n2 = b;
		this.w = w;
	}
	
	public int compareTo(Edge e) {
		if(this.w > e.w)
			return 1;
		else if (this.w < e.w) 
			return -1;
		else 
			return 0;
	}
	
}
