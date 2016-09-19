package ca.polymtl.inf4410.tp1.client;

import java.rmi.AccessException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

import ca.polymtl.inf4410.tp1.shared.ServerInterface;

public class Client {
	public static void main(String[] args) {
		String distantHostname = null;
		String exponentValue = "0";

		if (args.length > 0) {
			distantHostname = args[0];	
			
			if(args.length > 1){
                exponentValue = args[1];
			}
		}

		Client client = new Client(distantHostname);
		client.run(Integer.parseInt(exponentValue));
	}

	FakeServer localServer = null; // Pour tester la latence d'un appel de
									// fonction normal.
	private ServerInterface localServerStub = null;
	private ServerInterface distantServerStub = null;

	public Client(String distantServerHostname) {
		super();

		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		localServer = new FakeServer();
		localServerStub = loadServerStub("127.0.0.1");

		if (distantServerHostname != null) {
			distantServerStub = loadServerStub(distantServerHostname);
		}
	}

	private void run(int exponentValue) {
        if(exponentValue < 0){
            System.out.println("Exponent value is invalid, lower than 0");
            return;
        }
	
        for(int exponentCounter = 0; exponentCounter < exponentValue; ++exponentCounter)
        {
            System.out.println("******** Speed test for a payload of 10 ^ " + exponentCounter + " bytes ********");
            int payloadSize = (int) Math.pow(10,exponentCounter);
            byte[] payload = new byte[payloadSize];
            payload[0] = 3;
            
            System.out.println(payload.length);
            
            appelNormal(payload);

            if (localServerStub != null) {
                appelRMILocal(payload);
            }

            if (distantServerStub != null) {
                appelRMIDistant(payload);
            }
             System.out.println("********************************");
        }        

	}

	private ServerInterface loadServerStub(String hostname) {
		ServerInterface stub = null;

		try {
			Registry registry = LocateRegistry.getRegistry(hostname);
			stub = (ServerInterface) registry.lookup("server");
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

	private void appelNormal(byte[] payload) {
		long start = System.nanoTime();
		int result = localServer.speedTest(payload);
		long end = System.nanoTime();

		System.out.println("Temps écoulé appel normal: " + (end - start)
				+ " ns");
		System.out.println("Résultat appel normal: " + result);
	}

	private void appelRMILocal(byte[] payload) {
		try {
			long start = System.nanoTime();
			int result = localServerStub.speedTest(payload);
			long end = System.nanoTime();

			System.out.println("Temps écoulé appel RMI local: " + (end - start)
					+ " ns");
			System.out.println("Résultat appel RMI local: " + result);
		} catch (RemoteException e) {
			System.out.println("Erreur: " + e.getMessage());
		}
	}

	private void appelRMIDistant(byte[] payload) {
		try {
			long start = System.nanoTime();
			int result = distantServerStub.speedTest(payload);
			long end = System.nanoTime();

			System.out.println("Temps écoulé appel RMI distant: "
					+ (end - start) + " ns");
			System.out.println("Résultat appel RMI distant: " + result);
		} catch (RemoteException e) {
			System.out.println("Erreur: " + e.getMessage());
		}
	}
}
