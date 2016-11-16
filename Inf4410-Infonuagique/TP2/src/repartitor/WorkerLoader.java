package repartitor;

import java.rmi.AccessException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

import contract.IWorker;

// Helper pour load les Worker-Proxy
public class WorkerLoader {
	
	
	public static IWorker LoadLocalRepartitor(){
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}	
		
		IWorker stub = null;

		try {
			Registry registry = LocateRegistry.getRegistry();
			stub = (IWorker) registry.lookup("server");
		} catch (NotBoundException e) {
			System.out.println("Erreur: Le nom '" + e.getMessage()
					+ "' n'est pas défini dans le registre.");
		} catch (AccessException e) {
			System.out.println("Erreur: " + e.getMessage());
		} catch (RemoteException e) {
			System.out.println("Erreur: " + e.getMessage());
		}

		return stub;
		
	}
	
	public static IWorker LoadRepartitor(String hostname) throws Exception {
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}
		
		IWorker stub = null;

		try {
			Registry registry = LocateRegistry.getRegistry(hostname, 5000);
			stub = (IWorker) registry.lookup("server");
		} catch (NotBoundException e) {
			System.out.println("Erreur: Le nom '" + e.getMessage()
					+ "' n'est pas défini dans le registre.");
		} catch (AccessException e) {
			System.out.println("Erreur: " + e.getMessage());
		} catch (RemoteException e) {
			System.out.println("Erreur: " + e.getMessage());
		}

		return stub;
	}
}
