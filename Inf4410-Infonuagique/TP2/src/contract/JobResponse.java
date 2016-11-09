package contract;

// Ce que le worker doit retourner en reponse à un job
// Contient détail
public class JobResponse implements java.io.Serializable {

	private static final long serialVersionUID = 1L;
	
	public ResultStatus Status;
	public int response;
	public String Details;
}
