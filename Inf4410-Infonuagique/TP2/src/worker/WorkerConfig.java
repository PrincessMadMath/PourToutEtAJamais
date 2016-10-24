package worker;

public class WorkerConfig {
	
	public int Capacity;
	public double ProbabilityFalseResult;
	public String ServerAddress;
	public String Port;
	
	public WorkerConfig(int capacity, double falseProb, String serverAddress, String port){
		Capacity = capacity;
		ProbabilityFalseResult = falseProb;
		ServerAddress = serverAddress;
		Port = port;		
	}
}
