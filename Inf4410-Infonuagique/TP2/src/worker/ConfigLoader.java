package worker;

import java.io.File;
import java.nio.file.Files;
import java.util.HashMap;
import java.util.Map;


public class ConfigLoader {
	
	private static Map<String, WorkerConfig> configs;
	
	private static void init()
	{
		if(configs == null)
		{
			configs = new HashMap<String, WorkerConfig>();
			configs.put("bad_5", new WorkerConfig(5, 0.5, "", ""));
			configs.put("good_5", new WorkerConfig(5, 0, "", ""));
		}
	}

	public static WorkerConfig LoadConfig(String configPath) throws Exception
	{
		init();
		
		return configs.get(configPath);		
	}
	
	
	
}
