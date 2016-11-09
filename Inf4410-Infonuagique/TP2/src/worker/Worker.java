package worker;

import java.rmi.ConnectException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.Random;

import contract.IWorker;
import contract.Job;
import contract.JobResponse;
import contract.Operation;
import contract.ResultStatus;

// Worker/Server qui vont faire les operations
public class Worker implements IWorker {
	
	private WorkerConfig _config;
	
	public static void main(String[] args) throws Exception {
		if (args.length != 2) {
			throw new Exception("Need 2 arguments, received : " + args.length );
		}
		
		String configPath = args[0];
		String host = args[1];
		
		Worker worker = new Worker(configPath, host);
		
		worker.run();
			
	}
	
	
	public Worker(String configPath, String host) throws Exception{
		super();
		
		_config = ConfigLoader.LoadConfig(configPath, host);	
	}
	
	// Va s'enregistrer au rmi
	private void run() {
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		try {
			IWorker stub = (IWorker) UnicastRemoteObject.exportObject(this, 0);

			Registry registry = LocateRegistry.getRegistry(_config.ServerAddress,_config.Port);
			registry.rebind("server", stub);
			System.out.println("Server ready.");
		} catch (ConnectException e) {
			System.err
					.println("Impossible de se connecter au registre RMI. Est-ce que rmiregistry est lancé ?");
			System.err.println();
			System.err.println("Erreur: " + e.getMessage());
		} catch (Exception e) {
			System.err.println("Erreur: " + e.getMessage());
		}
	}
	
	// Determine selon workload si prend la job
	private Boolean doesTakeJob(int operationCount){
		double refusalTreshold = (operationCount - _config.Capacity) / (4*_config.Capacity);
		
		Random r = new Random();
		double value = r.nextDouble();
		
		Boolean mustTakeJob = value > refusalTreshold;
		
		return mustTakeJob;
	}
	
	// Determine si doit mentir (basé sur probs)
	private Boolean mustTroll()
	{
		double trollTreshold =  _config.ProbabilityFalseResult;
		
		Random r = new Random();
		double value = r.nextDouble();
		
		Boolean mustTroll = value < trollTreshold;
		
		return mustTroll;
	}

	/***************** Implementation du worker *****************/
	
	public JobResponse Execute(Job job) throws RemoteException {
		JobResponse response = new JobResponse();
		
		System.out.println("Receive " + job.Operations.size() + " operations");
		
		if(!doesTakeJob(job.Operations.size())){
			response.Status = ResultStatus.Deny;
			response.Details = "Deny, because to much work and I am lazy!"; 
			return response;
		}
		
		if(mustTroll())
		{
			response.Status = ResultStatus.Success;
			
			Random rn = new Random();
			int trollResponse = rn.nextInt();
			
			response.response = trollResponse;
			response.Details = "Totally not trolling, I swear!";
			return response;
			
		}
		
		response.response = DoJob(job);
		response.Details = "I did my duty!";
		
		return response;
	}
	
	public int DoJob(Job job)
	{
		int sum = 0;
		
		for(Operation op : job.Operations)
		{
			switch(op.type){
				case Pell:
					sum += Operations.pell(op.operand);
					break;
				case Prime:
					sum += Operations.prime(op.operand);
					break;
			}
			sum %= 4000;
		}
		
		return sum;
	}
	
	public String SayHelloWorld() throws RemoteException {
		System.out.println("Not SayingHelloWorld");
		return "No!";
	}
		
}
