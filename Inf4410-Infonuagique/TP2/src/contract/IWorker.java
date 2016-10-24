package contract;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface IWorker extends Remote{
	
	JobResponse Execute(Job job) throws RemoteException;
}