package ca.polymtl.inf4410.tp1.shared;

import java.util.UUID;

public class FileMeta implements java.io.Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	public FileMeta(String name, UUID ownerId){
		Name = name;
		OwnerId = ownerId;
	}
	
	public String Name;
	public UUID OwnerId;
}
