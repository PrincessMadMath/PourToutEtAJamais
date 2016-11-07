package repartitor;

import java.util.ArrayList;
import java.util.List;

import contract.IWorker;
import contract.Job;

public class JobProgress {
	
	public JobProgress(Job job)
	{
		Job = job;
		Result = new ArrayList<Integer>();
		Workers = new ArrayList<IWorker>();
	}
		
	public List<Integer> Result;	
	//
	public List<IWorker> Workers;
	
	public Job Job;
}
