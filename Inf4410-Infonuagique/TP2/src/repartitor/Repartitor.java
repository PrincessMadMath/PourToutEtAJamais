package repartitor;

import java.io.IOException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;

import contract.IWorker;
import contract.Job;
import contract.JobResponse;
import contract.Operation;
import contract.ResultStatus;


public class Repartitor {
	
	private List<IWorker> AvailableWorkers= new ArrayList<IWorker>();
	private List<JobProgress> JobsToDo = new ArrayList<JobProgress>();
	
	private int result = 0;
	
	private final int InitialJobSize = 100;
	private final static int SecureCountCheck = 2;
	
	
	private static int CountCheck = 1;
	
	private Semaphore workerCount;
	
	
	public static void main(String[] args) throws Exception {
		
		if (args.length != 2) {
			throw new Exception("2 arguments, received : " + args.length );
		}
		
		String operationsPath = args[0];
		
		String isSecure = args[1];
		
		if(isSecure != "1")
		{
			CountCheck = SecureCountCheck;
			System.out.println("Not trusting worker, will check: " + SecureCountCheck);
		}
		
		
		Repartitor repartitor = new Repartitor(operationsPath);
				
		repartitor.Dispatch();
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
		
		workerCount = new Semaphore(AvailableWorkers.size());
		
		InitJobs(path);
	}
	
	private void InitJobs(String path) throws IOException{
		List<Operation> allOperations = JobLoader.getJob(path);
		
		List<List<Operation>> initialSplitOperation = JobUtils.chopped(allOperations, InitialJobSize);
		
		for(List<Operation> operations : initialSplitOperation)
		{
			Job job = new Job(operations);
			JobsToDo.add(new JobProgress(job));
		}
		
	}
	
	private void Dispatch() throws Exception
	{
		while(!JobsToDo.isEmpty())
		{
			workerCount.acquire();
			
			IWorker worker = AvailableWorkers.get(0);
			AvailableWorkers.remove(worker);
			
			JobProgress jobToDo = GetNextJob(worker);
			
			if(jobToDo == null)
			{
				System.out.println("Can't find job for worker");
				
				workerCount.release();
				AvailableWorkers.add(worker);
			}
			else
			{			
				jobToDo.Workers.add(worker);
				
				Runnable runner = createTask(worker, jobToDo);
				runner.run();				
			}
			
		}
		
		System.out.println(result);
	}
	
	private Runnable createTask(final IWorker worker, final JobProgress jobProgress)
	{
	    Runnable aRunnable = new Runnable(){
	        public void run(){
	        	try {
					DoJob(worker, jobProgress);
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
	        }
	    };

	    return aRunnable;
	}
	
	private JobProgress GetNextJob(IWorker worker)
	{
		for(JobProgress possibleJob : JobsToDo)
		{
			Boolean hadNotWorkOnIt = !possibleJob.Workers.contains(worker);
			Boolean freeToUse = possibleJob.Workers.size() < CountCheck;
			
			if(hadNotWorkOnIt && freeToUse)
			{
				return possibleJob;
			}
		}
		
		return null;
	}
	
	private void DoJob(IWorker worker, JobProgress jobToDo) throws Exception{
		JobResponse response = worker.Execute(jobToDo.Job);
		
		// If too big, split job
		if(response.Status == ResultStatus.Deny)
		{			
			JobsToDo.remove(jobToDo);
			
			List<List<Operation>> splitedOperations = JobUtils.split(jobToDo.Job.Operations);
			
			for(List<Operation> operations : splitedOperations)
			{
				Job job = new Job(operations);
				JobsToDo.add(new JobProgress(job));
			}
		}
		else{
			
			// lock
			jobToDo.Result.add(response.response);
			
			
			if(jobToDo.Result.size() >= CountCheck)
			{
				// Add result
				if(IsResultLegit(jobToDo))
				{
					JobsToDo.remove(jobToDo);
					result += response.response;
					result %= 4000;
				}
				else
				{
					System.out.println("Detect trolling! D:<");
					ResetJobProgress(jobToDo);
				}
			}
			
		}
		
		// lock
		AvailableWorkers.add(worker);
		workerCount.release();		
	}
	
	private void ResetJobProgress(JobProgress jobToReset)
	{
		jobToReset.Result.clear();
		jobToReset.Workers.clear();
	}
	
	private Boolean IsResultLegit(JobProgress jobToDo)
	{
		int result = jobToDo.Result.get(0);
		
		for(int possibleResult : jobToDo.Result)
		{
			if(result != possibleResult)
			{
				return false;
			}
		}
		
		return true;
	}
	
}
