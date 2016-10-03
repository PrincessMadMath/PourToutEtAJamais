package ca.polymtl.inf4410.tp1.shared;

public class FileData  implements java.io.Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	public FileData(String name, byte[] data, String md5)
	{
		Name = name;
		Data = data;
		Md5 = md5;
	}
	
	public String Name;
	public byte[] Data;
	public String Md5;
}
