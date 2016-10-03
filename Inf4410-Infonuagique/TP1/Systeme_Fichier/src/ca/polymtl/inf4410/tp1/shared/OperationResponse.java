package ca.polymtl.inf4410.tp1.shared;

public class OperationResponse implements java.io.Serializable {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	public OperationResponse(Boolean isSuccess, String details)
	{
		IsSuccess = isSuccess;
		Details = details;
	}
	
	public Boolean IsSuccess;
	public String Details;
}
