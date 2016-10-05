package ca.polymtl.inf4410.tp1.server;

import java.io.File;
import java.io.IOException;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.rmi.ConnectException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import javax.lang.model.element.Element;

import ca.polymtl.inf4410.tp1.shared.FileData;
import ca.polymtl.inf4410.tp1.shared.FileMeta;
import ca.polymtl.inf4410.tp1.shared.OperationResponse;
import ca.polymtl.inf4410.tp1.shared.ServerInterface;

public class Server implements ServerInterface {

	public static void main(String[] args) throws Exception {
		Server server = new Server();
		server.init();
		server.run();
	}

	private String folderName = "serverFolder";
	private Map<String, FileMeta> filesMata = new HashMap<String, FileMeta> ();
	
	public Server() throws Exception {
		super();
		
		// Create folder if was not existing
		new File(folderName).mkdirs();
	}
	
	private void init(){
		File folder = new File(folderName);
		File[] listOfFiles = folder.listFiles();

	    for (int i = 0; i < listOfFiles.length; i++) {
	      if (listOfFiles[i].isFile()) {
	    	String name = listOfFiles[i].getName();
	    	FileMeta fileMeta = new FileMeta(listOfFiles[i].getName(), null);
	    	filesMata.put(name, fileMeta);
	      } 
	    }
	}
	
	private String GetPath(String fileName)
	{
		return folderName + "/" + fileName;
	}
	
	private void run() {
		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		try {
			ServerInterface stub = (ServerInterface) UnicastRemoteObject
					.exportObject(this, 0);

			Registry registry = LocateRegistry.getRegistry();
			registry.rebind("server", stub);
			System.out.println("Server ready.");
		} catch (ConnectException e) {
			System.err
					.println("Impossible de se connecter au registre RMI. Est-ce que rmiregistry est lancé ?");
			System.err.println();
			System.err.println("Erreur: " + e.getMessage());
		} catch (Exception e) {
			System.err.println("Erreur: " + e.getMessage());
		}
	}

	/*
	 * Implémentation du serveur
	 */
	
	@Override
	public UUID generateclientid() throws RemoteException {
		return UUID.randomUUID();
	}

	@Override
	public OperationResponse create(String name) throws RemoteException {
		Path path = Paths.get(GetPath(name));
		if(Files.exists(path)) {
			return new OperationResponse(false, "Cannot create: " + name + ", because file already exist");
		}
		else{
			try {
				Files.write(path, "".getBytes());
				
				FileMeta fileMeta = new FileMeta(name, null);
		    	filesMata.put(name, fileMeta);
				
				return new OperationResponse(true, "Sucessefully created file" + name);
			} catch (IOException e) {
				return new OperationResponse(false, "Failed creating file: " + name + "\n" + e.getMessage());
			}
		}
	}
	

	@Override
	public List<FileMeta> list() throws RemoteException {
		List<FileMeta> files = new ArrayList<FileMeta>();
		
		for(Map.Entry<String, FileMeta> entry : filesMata.entrySet())
		{
			files.add(entry.getValue());
		}
		
		return files;
	}

	@Override
	public List<FileData> syncLocalDir() throws RemoteException {
		

		List<FileData> filesData = new ArrayList<FileData> ();
		
		File folder = new File(folderName);
		File[] listOfFiles = folder.listFiles();

	    for (int i = 0; i < listOfFiles.length; i++) {
	      if (listOfFiles[i].isFile()) {
	    	String name = listOfFiles[i].getName();	    	
   	
			try {

		    	filesData.add(GetFileData(name));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
	      } 
	    }
		
		return filesData;
	}
		
	private String GetMd5(byte[] data){
		MessageDigest md;
		try {
			md = MessageDigest.getInstance("MD5");
		} catch (NoSuchAlgorithmException e1) {
			// TODO Auto-generated catch block
		}
	
		BigInteger bigInt = new BigInteger(1, data);
		return bigInt.toString(16);
	}
	
	private Boolean IsFileExist(String fileName){
		Path path = Paths.get(GetPath(fileName));
		return Files.exists(path); 
	}	
	
	private FileData GetFileData(String name) throws IOException
	{		
		byte[] data = Files.readAllBytes(Paths.get(GetPath(name)));
    	String md5 = GetMd5(data);
    	return new FileData(name, data, md5);		
	}

	@Override
	public FileData get(String name, String checksum) throws RemoteException 
	{	
		FileData currentFile;
		try {
			if(IsFileExist(name))
			{			
				currentFile = GetFileData(name);
				
				// If client have latest version
				if(currentFile.Md5.equals(checksum))
				{
					return null;
				}
				else{
					return currentFile;
				}
			}
			else{
				throw new RemoteException("File: " + name + " does not exist."); 
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	@Override
	public OperationResponse lock(String name, UUID clientId, String checksum)
	{
		FileMeta wantedFile = filesMata.get(name);
		
		if(wantedFile == null){
			return new OperationResponse(false, "Can't lock a file that doesn't exist: " + name);
		}
		
		synchronized(wantedFile){
			if(wantedFile.OwnerId != null && wantedFile.OwnerId != clientId)
			{
				return new OperationResponse(false, "File is already locked by someone else" + name);
			}
			else
			{
				try
				{
					wantedFile.OwnerId = clientId;
					FileData currentFile = GetFileData(name);
					if(currentFile.Md5.equals(checksum))
					{
						return new OperationResponse(true, "Lock succeed on : " + name);
					}
					else
					{
						return new OperationResponse(true, "Lock succeed on : " + name + ", but you don't have the latest version");
					}
				}
				catch (IOException e) {
					e.printStackTrace();
					return new OperationResponse(true, "Error while locking: " + e);
				}			
			}
		}
	}

	@Override
	public OperationResponse push(String name, byte[] data, UUID clientId)
	{
		FileMeta wantedFile = filesMata.get(name);
		
		if(wantedFile == null){
			return new OperationResponse(false, "This file does not exist: " + name);
		}
		
		synchronized(wantedFile){
			if(!wantedFile.OwnerId.equals(clientId))
			{
				return new OperationResponse(false, "You don't have the required lock to push on the file: " + name + "| ownwer : " + wantedFile.OwnerId + "| you: " + clientId);
			}
			else
			{
				try
				{
					Path path = Paths.get(GetPath(name));
					Files.write(path, data);
					wantedFile.OwnerId = null;
					
					return new OperationResponse(true, "Update pushed on: " + name);
				} 
				catch (IOException e) {
				return new OperationResponse(false, "Failed creating file: " + name + "\n" + e.getMessage());
				}
			}
		}	
	}
	
}
