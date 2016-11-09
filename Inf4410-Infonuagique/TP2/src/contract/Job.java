package contract;

import java.util.ArrayList;
import java.util.List;

// Representation d'une Job aka liste d'opérations
public class Job implements java.io.Serializable {
	
	private static final long serialVersionUID = 1L;
	
	

	public Job()
	{
		Operations = new ArrayList<Operation>();
	}
	
	public Job(List<Operation> operations)
	{
		Operations = operations;
	}
	
	public List<Operation> Operations;
}
