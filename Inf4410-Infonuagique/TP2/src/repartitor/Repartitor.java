package repartitor;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import contract.IWorker;
import contract.Job;
import contract.JobResponse;
import contract.Operation;
import contract.OperationType;


public class Repartitor {
	
	Job currentJob;
	
	public static void main(String[] args) throws Exception {
		
		if (args.length != 1) {
			throw new Exception("One and only one arguments, received : " + args.length );
		}
		
		String operationsPath = args[0];
		
		
		Repartitor repartitor = new Repartitor(operationsPath);
				
		repartitor.SimpleTest();
	}
	
	public Repartitor(String path) throws Exception{
		// 01
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.161"));
		// 02
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.162"));
		// 03
		//AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.163"));
		// 04
		//AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.164"));
		
		currentJob = JobLoader.getJob(path);
	}
	
	private List<IWorker> AvailableWorkers= new ArrayList<IWorker>();
	
	private void SimpleTest() throws RemoteException{
		for(IWorker worker : AvailableWorkers)
		{
			JobResponse response = worker.Execute(currentJob);
			System.out.println(response.Details + " : " + response.response);
		}
	}
	
	

}
