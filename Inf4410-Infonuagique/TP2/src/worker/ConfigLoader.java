package worker;

import java.util.HashMap;
import java.util.Map;


// Helper to start worker easily
public class ConfigLoader {
	
	private static Map<String, WorkerConfig> configs;
	
	private static void init()
	{
		if(configs == null)
		{
			configs = new HashMap<String, WorkerConfig>();
			configs.put("bad_5", new WorkerConfig(5, 0.5, "", 5000));
			configs.put("good_3", new WorkerConfig(3, 0, "", 5000));
			configs.put("good_5", new WorkerConfig(6, 0, "", 5000));
			configs.put("good_6", new WorkerConfig(6, 0, "", 5000));
			configs.put("good_9", new WorkerConfig(9, 0, "", 5000));
			configs.put("good_12", new WorkerConfig(12, 0, "", 5000));
			configs.put("hack", new WorkerConfig(10000, 0, "", 5000));
		}
	}

	public static WorkerConfig LoadConfig(String configPath, String host) throws Exception
	{
		init();
		
		WorkerConfig config = configs.get(configPath);
		config.ServerAddress = host;
		return config;
	}	
}
