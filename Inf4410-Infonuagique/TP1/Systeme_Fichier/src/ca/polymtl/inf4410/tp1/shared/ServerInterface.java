package ca.polymtl.inf4410.tp1.shared;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.util.*;

public interface ServerInterface extends Remote {
	UUID generateclientid() throws RemoteException;
	OperationResponse create(String name) throws RemoteException;
	List<FileMeta> list() throws RemoteException;
	List<FileData> syncLocalDir() throws RemoteException;
	FileData get(String name, String checksum) throws RemoteException;
	OperationResponse lock(String name, UUID clientId, String checksum)  throws RemoteException;
	OperationResponse push(String name, byte[] data, UUID clientId) throws RemoteException;
}
