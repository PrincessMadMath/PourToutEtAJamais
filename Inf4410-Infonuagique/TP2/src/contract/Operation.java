package contract;

// Representation d'une operation
public class Operation implements java.io.Serializable{
	private static final long serialVersionUID = 1L;
	public OperationType type;
	public int operand;
}
