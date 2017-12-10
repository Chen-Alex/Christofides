
public class Pair {
	int a;
	double b;
	public Pair(int a, double b) {
		this.a = a;
		this.b = b;
	}

	@Override
	public boolean equals(Object obj) {
//		if (this == obj)
//			return true;
//		if (obj == null)
//			return false;
//		if (getClass() != obj.getClass())
//			return false;
//		Pair other = (Pair) obj;
//		if (a != other.a)
//			return false;
//		if (Double.doubleToLongBits(b) != Double.doubleToLongBits(other.b))
//			return false;
//		return true;
		Pair p = (Pair) obj;
		if (this.a == p.a && this.b == p.b)
			return true;
		return false;
	}

	@Override
	public String toString() {
		return "Pair [a=" + a + ", b=" + b + "]";
	}
}
