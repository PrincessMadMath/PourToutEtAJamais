package repartitor;

import java.util.ArrayList;
import java.util.List;

import contract.IWorker;
import contract.Job;

// Permet le suivi de la progression d'une job
public class JobProgress {
	
	public JobProgress(Job job)
	{
		Job = job;
		Result = new ArrayList<Integer>();
		Workers = new ArrayList<IWorker>();
	}
		
	// Resultat donné par les workers qui ont travaillé dessus (utile pour validation contre les trolls)
	public List<Integer> Result;	
	
	// Worker qui ont travaillé dessus (eviter que meme worker travaille 2 fois dessus)
	public List<IWorker> Workers;
	
	public Job Job;
	
	// Determiner si la job a ete abandonne et ne doit pas etre considere dans la reponse (si doit se faire split du au workload)
	public Boolean IsAborted = false;
}
