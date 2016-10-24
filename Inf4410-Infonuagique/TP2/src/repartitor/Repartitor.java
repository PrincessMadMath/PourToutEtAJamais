package repartitor;

import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.List;

import contract.IWorker;


public class Repartitor {
	
	
	public static void main(String[] args) throws Exception {
		Repartitor repartitor = new Repartitor();
		
		repartitor.SimpleTest();
	}
	
	public Repartitor(){
		AvailableWorkers.add(WorkerLoader.LoadLocalRepartitor());
	}
	
	private List<IWorker> AvailableWorkers= new ArrayList<IWorker>();
	
	private void SimpleTest() throws RemoteException{
		for(IWorker worker : AvailableWorkers)
		{
			System.out.println(worker.SayHelloWorld());
		}
	}

}
