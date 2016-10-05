package ca.polymtl.inf4410.tp1.client;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.math.BigInteger;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.rmi.AccessException;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.*;

import ca.polymtl.inf4410.tp1.shared.FileData;
import ca.polymtl.inf4410.tp1.shared.FileMeta;
import ca.polymtl.inf4410.tp1.shared.OperationResponse;
import ca.polymtl.inf4410.tp1.shared.ServerInterface;


public class Client {
	public static void main(String[] args) throws Exception {
		if (args.length < 2) {
			throw new Exception("Missing arguments: the minimum required is the ip and the operation name");
		}
		
		String distantHostname = args[0];
		String command = args[1];
		String[] commandArgs = Arrays.copyOfRange(args, 2, args.length);

		Client client = new Client(distantHostname);
			
		
		client.doOperation(command, commandArgs); 
	}
	
	// Proxy (local for testing purpose)
	private ServerInterface localServerStub = null;
	private ServerInterface distantServerStub = null;
	
	
	private String CloudFolderName = "dropbox_improved";
	private String ClientIdFile = "clientid.txt";
	
	private UUID clientId = null;
	
	// Easy switch for local and remote
	private ServerInterface getProxy(){
		return localServerStub;
	}
	
	
	// Constructor : init both proxy
	public Client(String distantServerHostname) throws Exception {
		super();

		if (System.getSecurityManager() == null) {
			System.setSecurityManager(new SecurityManager());
		}

		localServerStub = loadServerStub("127.0.0.1");

		if (distantServerHostname != null) {
			distantServerStub = loadServerStub(distantServerHostname);
		}
		
		String filePath = GetPath(ClientIdFile);
		Path path = Paths.get(filePath);
		if(Files.exists(path)) { 
		    List<String> texts = Files.readAllLines(path);
		    clientId = UUID.fromString(texts.get(0));
		}
		else{
			if(!(new File(CloudFolderName).mkdirs()))
			{
				throw new Exception("Failed creating sync folder");
			}
			PrintWriter writer = new PrintWriter(filePath, "UTF-8");
			clientId = getProxy().generateclientid();
			writer.println(clientId.toString());
			writer.close();
		}
	}
	
	private String GetPath(String fileName)
	{
		return CloudFolderName + "/" + fileName;
	}
	
	private void doOperation(String command, String[] args) throws Exception	{
		ServerInterface proxy = getProxy();
		
		OperationResponse response = null;
				
		switch(command.toLowerCase()){
		case "generateclientid":
			if (args.length != 0) {
				throw new Exception("Invalid arguments");
			}
			clientId = proxy.generateclientid();
			System.out.println(clientId);
			break;
			
		case "myid":
			System.out.println(clientId);
			break;
			
		case "create":
			if (args.length != 1) {
				throw new Exception("Invalid arguments");
			}
			response = proxy.create(args[0]);
			System.out.println(response.Details);
			break;
			
		case "list":
			if (args.length != 0) {
				throw new Exception("Invalid arguments");
			}
			List<FileMeta> list = proxy.list();
			for(FileMeta file : list){
				System.out.println("	Name: " + file.Name + "| Owner: " + file.OwnerId);
			}
			
			break;
			
		case "synclocaldir":
			if (args.length != 0) {
				throw new Exception("Invalid arguments");
			}
			
			List<FileData> dataList = proxy.syncLocalDir();
			
			for(FileData file : dataList){
				System.out.println("Download file: " + file.Name);
				WriteFile(file);
			}
			
			break;
			
		case "get":
			if (args.length != 1) {
				throw new Exception("Invalid arguments");
			}
			
			String name = args[0];
			
			// If file exist locally
			if(IsFileExist(name))
			{
				FileData localData = GetFileData(name);
				FileData remoteData = proxy.get(name, localData.Md5);
				
				// If not latest version
				if(remoteData != null)
				{
					WriteFile(remoteData);
					System.out.println("Download new version of file: " + name);

				}
				else
				{
					System.out.println("Already latest version of file: " + name);
				}		
			}
			else{
				FileData remoteData = proxy.get(name, "-1");
				if(remoteData != null)
				{
					WriteFile(remoteData);
					System.out.println("Downloaded file: " + name);
				}
				else{
					System.out.println("File does not exist on server!");
				}
			}
			
			
			break;
			
		case "lock":
			if (args.length != 1) {
				throw new Exception("Invalid arguments");
			}
			String fileNameToLock = args[0];
			
			if(IsFileExist(fileNameToLock))
			{			
				FileData fileToGetChecksum = GetFileData(fileNameToLock);
				response = proxy.lock(fileNameToLock, clientId, fileToGetChecksum.Md5);
				System.out.println(response.Details);
			}
			else
			{
				System.out.println("What are you trying to lock, I don't even... (you don't have the file: " + fileNameToLock + " locally mate!)");
			}

			
			break;
			
		case "push":
			if (args.length != 1) {
				throw new Exception("Invalid arguments");
			}
			
			String fileNameToPush = args[0];
			
			if(IsFileExist(fileNameToPush))
			{
				FileData fileToPush = GetFileData(fileNameToPush);
				response = proxy.push(fileNameToPush, fileToPush.Data ,clientId);
				System.out.println(response.Details);
			}
			else
			{
				System.out.println("What are you trying to push, I don't even... (you don't have the file: " + fileNameToPush+ " locally mate!)");
			}

			break;	
			
		default:
			throw new Exception("Unknown operation: " + command);
		}
	}
	
	private void WriteFile(FileData file) throws IOException
	{
		Path path = Paths.get(GetPath(file.Name));
		Files.write(path, file.Data);
	
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
	
	// Setup server proxy using the registry
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
}
