package contract;

import java.util.ArrayList;
import java.util.List;

public class Job implements java.io.Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public Job()
	{
		Operations = new ArrayList<Operation>();
	}
	
	public List<Operation> Operations;
}
