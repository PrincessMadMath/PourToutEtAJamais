package worker;

import java.rmi.ConnectException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

import contract.IWorker;
import contract.Job;
import contract.JobResponse;


public class Worker implements IWorker {
	
	public static void main(String[] args) throws Exception {
		if (args.length != 1) {
			throw new Exception("One and only one arguments, received : " + args.length );
		}
		
		String configPath = args[0];
		
		
		Worker worker = new Worker(configPath);
		
		worker.run();
			
	}
	
	private WorkerConfig _config;
	
	public Worker(String configPath) throws Exception{
		super();
		
		_config = ConfigLoader.LoadConfig(configPath);	
	}
	
	private void run() {
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		try {
			IWorker stub = (IWorker) UnicastRemoteObject.exportObject(this, 0);

			Registry registry = LocateRegistry.getRegistry();
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

	/***************** Implementation du worker *****************/
	
	@Override
	public JobResponse Execute(Job job) throws RemoteException {
		// TODO Auto-generated method stub
		return null;
	}
	
	// Todo: complete
	
}
