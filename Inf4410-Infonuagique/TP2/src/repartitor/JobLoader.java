package repartitor;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import contract.Job;
import contract.Operation;
import contract.OperationType;

// Helper pour loader le fichier d'operation en liste d'operation
public class JobLoader {

	public static List<Operation> getJob(String path) throws IOException
	{
		List<Operation>  operations = new ArrayList<Operation>();
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
				
				operations.add(operation);
			}
		}
		
		return operations;
	}
}