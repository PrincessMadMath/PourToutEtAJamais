package repartitor;

import contract.JobResponse;

// Helper
public class JobResponsePair {
	
	public JobProgress Job;
	public JobResponse Response;
	
	public JobResponsePair(JobProgress job, JobResponse response)
	{
		Job = job;
		Response = response;
	}
}
