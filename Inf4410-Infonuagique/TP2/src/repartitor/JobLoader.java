package repartitor;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import contract.Job;
import contract.Operation;
import contract.OperationType;

public class JobLoader {

	public static Job getJob(String path) throws IOException
	{
		Job job = new Job();
		try(BufferedReader br = new BufferedReader(new FileReader(path))){
			String line;
			while((line = br.readLine()) != null)
			{
				Operation operation = new Operation();
				List<String> split = Arrays.asList(line.split(" "));
				
				if(split.size() != 2)
				{
					throw new IOException("Wrong file!");
				}
				
				switch(split.get(0))
				{
					case "prime":
						operation.type = OperationType.Prime;
						break;
					case "pell":
						operation.type = OperationType.Pell;
						break;
				}
				
				operation.operand = Integer.parseInt(split.get(1));
				
				job.Operations.add(operation);
			}
		}
		
		return job;
	}
}