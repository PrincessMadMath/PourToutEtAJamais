package repartitor;

import java.io.IOException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import contract.IWorker;
import contract.Job;
import contract.JobResponse;
import contract.Operation;
import contract.ResultStatus;

// Client en charge de repartir les calculs aux workers
public class Repartitor {
	
	// Lock pour prevenir des modification concurrentes
	private Object workersListLock = new Object();
	private Object jobsListLock = new Object();
	private Object jobCompleteLock = new Object();
	
	// Liste des workers disponible a prendre une job
	private List<IWorker> AvailableWorkers= new ArrayList<IWorker>();
	// Semaphore indiquant quand il y a un worker de disponible
	private Semaphore workerCount;
	
	// Liste des jobs à effectué
	private List<JobProgress> JobsToDo = new ArrayList<JobProgress>();
	
	// Liste des réponses aux jobs a traité
	private List<JobResponsePair> CompleteJobToProcess = new ArrayList<JobResponsePair>();
	// Signale au thread qu'il y a une reponse a traite	
	private Semaphore JobCompleteCount = new Semaphore(0);
	
	// Semaphore indiquant quand le result est prête a être lu
	private Semaphore ResultComplete = new Semaphore(0);
	
	private int Result = 0;
	
	private final int InitialJobSize = 100;
	
	// Nb de fois que la reponse d'un worker doit être vérifier avec d'autre
	private static int CountCheck = 1;
	private final static int SecureCountCheck = 2;
	
	// Ip test
	String worker_ip = "132.207.12.162";
	
	
	public static void main(String[] args) throws Exception {
		
		if (args.length != 2) {
			throw new Exception("2 arguments, received : " + args.length );
		}
		
		String operationsPath = args[0];
		
		int isSecure = Integer.parseInt(args[1]);
		if(isSecure == 1)
		{
			CountCheck = SecureCountCheck;
			System.out.println("Not trusting worker, will check: " + CountCheck);
		}
		else{
			System.out.println("Will trust, will check: " + CountCheck);
		}
		
		Repartitor repartitor = new Repartitor(operationsPath);
		
		long start = System.nanoTime();   	
		repartitor.Dispatch();
		long elapsedTime = System.nanoTime() - start;
		System.out.println("Elapse time: " + elapsedTime/1000000 + " ms");
	}
	
	// On load les workers disponibles
	public Repartitor(String path) throws Exception{
		// 01
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.161"));
		// 02
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.162"));
		// 03
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.163"));
		// 04
		AvailableWorkers.add(WorkerLoader.LoadRepartitor("132.207.12.164"));
		
		workerCount = new Semaphore(AvailableWorkers.size());
		
		InitJobs(path);
	}
	
	// Va loader la liste des opérations
	private void InitJobs(String path) throws IOException{
		List<Operation> allOperations = JobLoader.getJob(path);
		
		List<List<Operation>> initialSplitOperation = JobUtils.chopped(allOperations, InitialJobSize);
		
		for(List<Operation> operations : initialSplitOperation)
		{
			Job job = new Job(operations);
			JobsToDo.add(new JobProgress(job));
		}
		
	}
	
	// Single thread: va dispatcher les jobs au workers
	private void Dispatch() throws Exception
	{
		// Commence le reponse handler
		Runnable responseHandler = startResponseHandler();
		new Thread (responseHandler).start(); 
		
		// Doit dispatcher job tant qu'il y en a
		while(!JobsToDo.isEmpty())
		{
			// Attend qu'un worker soit disponible
			workerCount.acquire();
			
			IWorker worker = AvailableWorkers.get(0);
			AvailableWorkers.remove(worker);

			// Trouve une job pour le worker
			JobProgress jobToDo = GetNextJob(worker);
			
			if(jobToDo == null)
			{
				// Put worker at the end of the list
				AvailableWorkers.add(worker);
				workerCount.release();
			}
			else
			{
				// Indique que ce worker travaille sur cette job
				synchronized(workersListLock) {
					jobToDo.Workers.add(worker);
				}
				
				// Lance le worker
				Runnable runner = createTask(worker, jobToDo);
				new Thread (runner).start(); 			
			}
			
		}
		
		ResultComplete.acquire();
		System.out.println(Result);
	}
	
	// Choisi une job pour le worker (ne doit pas avoir travaillé dessus)
	private JobProgress GetNextJob(IWorker worker)
	{
		synchronized(jobsListLock) {
			for(JobProgress possibleJob : JobsToDo)
			{
				Boolean hadNotWorkOnIt = !possibleJob.Workers.contains(worker);
				Boolean freeToUse = possibleJob.Workers.size() < CountCheck;
				
				if(hadNotWorkOnIt && freeToUse)
				{
					return possibleJob;
				}
			}
		}
		
		return null;
	}
	
	
	/************* Response handler *************/
	
	private Runnable startResponseHandler()
	{
	    Runnable aRunnable = new Runnable(){
	        public void run(){
	        	try {
	        		HandleJobResponse();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
	        }
	    };

	    return aRunnable;
	}
	
	// Loop to consume finish job (single threaded)
	private void HandleJobResponse() throws Exception
	{
		while(!JobsToDo.isEmpty())
		{		
			// Wait for a job to be available
			JobCompleteCount.acquire();
			
			JobResponsePair pair;
			synchronized(jobCompleteLock) {
				pair = CompleteJobToProcess.get(0);
				CompleteJobToProcess.remove(pair);
			}
			
			JobResponse response = pair.Response;
			JobProgress jobToDo = pair.Job;
			
			// If too big, split job
			if(response.Status == ResultStatus.Deny)
			{	
				// Verifie si pas deja aborted par un autre worker : Sépare liste operations en 2 (cree 2 job, enleve la courante)
				if(!jobToDo.IsAborted)
				{
					List<List<Operation>> splitedOperations = JobUtils.split(jobToDo.Job.Operations);
					
					for(List<Operation> operations : splitedOperations)
					{
						Job job = new Job(operations);
						
						synchronized(jobsListLock) {
							JobsToDo.add(new JobProgress(job));
						}
					}
					
					synchronized(jobsListLock) {
						JobsToDo.remove(jobToDo);
						jobToDo.IsAborted = true;
					}
				}
			}
			// Sinon, doit verifier que reponse legit avant de l'ajouter au résultat
			else{
				synchronized(jobsListLock) {
					jobToDo.Result.add(response.response);
				}
				
				// Si assez de reponse obtenu pour cette job (depend du mode secure on non)
				if(jobToDo.Result.size() >= CountCheck)
				{
					// Add result if legit
					if(IsResultLegit(jobToDo))
					{
						Result += response.response;
						Result %= 4000;
						
						// Remove job that is completed
						synchronized(jobsListLock) {
							JobsToDo.remove(jobToDo);
						}
						
					}
					else
					{
						System.out.println("Detect trolling!");
						
						// Si trolled la job doit etre recommence a 0
						synchronized(jobsListLock) {
							ResetJobProgress(jobToDo);
						}
					}
				}
			}
		}
		
		ResultComplete.release();
	}
	
	// Lorsqu'une job est victime de troll faut reset tout!
	private void ResetJobProgress(JobProgress jobToReset)
	{
		jobToReset.Result.clear();
		jobToReset.Workers.clear();
	}
	
	// Verifie si le resultat semble legitime
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

	
	/************* Worker task *************/
	
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
	
	// Execute la job sur le worker-proxy, si crash tente re-connecter, 
	// sinon met reponse sur liste et rend worker a nouveau disponible
	private void DoJob(IWorker worker, JobProgress jobToDo) throws Exception{
			
			try{
				JobResponse response = worker.Execute(jobToDo.Job);
				
				synchronized(jobCompleteLock) {
					CompleteJobToProcess.add(new JobResponsePair(jobToDo, response));
				}
				
				// Signale que reponse pret a traite
				JobCompleteCount.release();
			}
			catch(RemoteException e)
			{
				// Va tenter de reconnecter
				
				synchronized(jobsListLock) {
				// Finalement ce worker n'a pas repondu a la job
				jobToDo.Workers.remove(jobToDo);
				}
				
				Runnable reconnectTask = tryReconnect(worker_ip);
				new Thread (reconnectTask).start(); 
			}
			
			synchronized(workersListLock) {
				AvailableWorkers.add(worker);
				workerCount.release();	
			}
			
	}
	

	
	
	/************* Reconnection task *************/
	// (todo: create WorkerProxy and move it in)
	
	private Runnable tryReconnect(final String ip)
	{
	    Runnable aRunnable = new Runnable(){
	        public void run(){
	        	try {
	        		TimeUnit.SECONDS.sleep(1);
	        		synchronized(workersListLock) {
	        			AvailableWorkers.add(WorkerLoader.LoadRepartitor(ip));
	        			workerCount.release();	
	        		}
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
	        }
	    };

	    return aRunnable;
	}
	
	
}
