package worker;

public class WorkerConfig {
	
	public int Capacity;
	public double ProbabilityFalseResult;
	public String ServerAddress;
	public int Port;
	
	public WorkerConfig(int capacity, double falseProb, String serverAddress, int port){
		Capacity = capacity;
		ProbabilityFalseResult = falseProb;
		ServerAddress = serverAddress;
		Port = port;		
	}
}
