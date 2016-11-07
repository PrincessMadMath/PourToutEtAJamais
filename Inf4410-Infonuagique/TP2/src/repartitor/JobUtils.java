package repartitor;

import java.util.ArrayList;
import java.util.List;

public class JobUtils {
	
	static <T> List<List<T>> chopped(List<T> list, final int L) {
	    List<List<T>> parts = new ArrayList<List<T>>();
	    final int N = list.size();
	    for (int i = 0; i < N; i += L) {
	        parts.add(new ArrayList<T>(
	            list.subList(i, Math.min(N, i + L)))
	        );
	    }
	    return parts;
	}
	
	// Not handling size of 1
	static <T> List<List<T>> split(List<T> list) throws Exception
	{
		if(list.size() < 2){
			throw new Exception("Not splittable list");
		}
		
		int halfSize = list.size() % 2 == 0? list.size()/2 : list.size()/2 + 1;
			
		return chopped(list, halfSize);
	}
}
