#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FILE_NAME CONFIG_PATH"app.cfg"

// Configuration tag constants
#define CFG_TAG_TRACE_LEVEL         "debuglevel"
#define CFG_TAG_TRACEFILE_SIZE      "tracefilesize"
#define CFG_TAG_TRACEFILE_COUNT     "tracefilecount"
#define CFG_TAG_TRACEFILE_NAME      "tracefilename"
#define CFG_TAG_TEST_INT            "testint"
#define CFG_TAG_TEST_STR            "teststr" 

typedef struct{
    int debuglevel;
    int tracefilesize;
    int tracefilecount;
    char tracefilename[20];
    int testint;
    char teststr[20];
}App_Configurations_s;


struct Config_Params_s {
    void* var_addr;
    int var_len;
    int var_type;
    const char* param;
    const char* def_value;

    // Constructor
    Config_Params_s(void* addr, int len, int type, const char* param, const char* def_value)
        : var_addr(addr), var_len(len), var_type(type), param(param), def_value(def_value) {}
};

class Config{
    private:
        static Config s_instance;
        sem_t configFileLock;
        std::string configFileName = CONFIG_FILE_NAME;
        std::vector<Config_Params_s> settings;
        
        int getConfigCountInConfigFile();
        void addNewConfigurations();

        void loadDefaultConfigurations();
        void loadConfig();
        void parseLine(const std::string& line);
        void logAppConfigurations();
        void saveConfig();
        
    public:
        Config();
        ~Config();
        static Config *getInstance();    

        App_Configurations_s configs;

        void initializeConfig();
        void updateConfiguration(const char *paramName,const char *newValue);

};

#endif // CONFIG_H
