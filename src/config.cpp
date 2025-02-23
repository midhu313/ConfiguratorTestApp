#include "includes.h"

Config Config::s_instance;

Config::Config() {
     // Explicitly initialize settings with individual assignments
    settings.push_back(Config_Params_s((void*)&configs.debuglevel, sizeof(int), INT_TYPE, CFG_TAG_TRACE_LEVEL, "0"));
    settings.push_back(Config_Params_s((void*)&configs.tracefilesize, sizeof(int), INT_TYPE, CFG_TAG_TRACEFILE_SIZE, "300000"));
    settings.push_back(Config_Params_s((void*)&configs.tracefilecount, sizeof(int), INT_TYPE, CFG_TAG_TRACEFILE_COUNT, "5"));
    settings.push_back(Config_Params_s((void*)&configs.tracefilename, 20, STRING_TYPE, CFG_TAG_TRACEFILE_NAME, "testfilename"));
    settings.push_back(Config_Params_s((void*)&configs.testint, sizeof(int), INT_TYPE, CFG_TAG_TEST_INT, "500"));
    settings.push_back(Config_Params_s((void*)&configs.teststr, 20, STRING_TYPE, CFG_TAG_TEST_STR, "Thisisteststr"));
    settings.push_back(Config_Params_s(nullptr, 0, 0, "", ""));
    mkdir(CONFIG_PATH,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    sem_init(&configFileLock,0,1);
}

Config::~Config(){
    sem_close(&configFileLock);
    sem_destroy(&configFileLock);
}

Config *Config::getInstance(){
    return &s_instance;
}

void Config::initializeConfig(){
    int cfg_inpgm_count  = (int)settings.size();
    int cfg_infile_count = getConfigCountInConfigFile();
    std::cout<<"Config inFileCount:"<<cfg_infile_count<<" inPgmCount:"<<cfg_inpgm_count<<std::endl;
    if(cfg_infile_count == 0){
        std::cout<<"No configuration File detected!"<<std::endl;
        loadDefaultConfigurations();
    }else if(cfg_infile_count==(cfg_inpgm_count-1))
        std::cout<<"No new configurations detected!"<<std::endl;
    else if(cfg_infile_count<(cfg_inpgm_count-1)){
        std::cout<<"New "<<((cfg_inpgm_count-1)-cfg_infile_count)<<"configurations detected!"<<std::endl;
        addNewConfigurations();
    }else{
        std::cout<<"Existing Configurations deleted!!"<<std::endl;
        loadDefaultConfigurations();
    }
    loadConfig();
    saveConfig();
}

void Config::loadDefaultConfigurations(){
    // Create a new config file with default values if the file doesn't exist
    std::ofstream configFile(configFileName);
    if (configFile.is_open()) {
        for (const auto& setting : settings) {
            if (setting.var_addr == nullptr) continue;  // Skip empty slots
            configFile << setting.param << "=" << setting.def_value << std::endl;
        }
        configFile.close();
    } else {
        std::cerr << "Failed to create config file!" << std::endl;
    }
}

void Config::loadConfig(){
    // Lock the semaphore before reading the file
    sem_wait(&configFileLock);
    // Load the config file and parse it line by line
    std::ifstream configFile(configFileName);
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            parseLine(line);
        }
        configFile.close();
    } else {
        std::cerr << "Failed to open config file for reading!" << std::endl;
    }
    // Unlock the semaphore after reading
    sem_post(&configFileLock);
}

void Config::parseLine(const std::string& line){
    // Parse a line from the config file (format: key=value)
    std::istringstream stream(line);
    std::string key, value;
    if (std::getline(stream, key, '=') && std::getline(stream, value)) {
        for (auto& setting : settings) {
            if (setting.param == key) {
                // Assign the value to the appropriate address based on type
                switch (setting.var_type) {
                    case INT_TYPE:
                        *(int*)setting.var_addr = std::stoi(value);
                        break;
                    case STRING_TYPE:
                        memcpy(setting.var_addr,value.c_str(),setting.var_len);
                        break;
                    case LONG_TYPE:
                        *(long*)setting.var_addr = std::stol(value);
                        break;
                    case FLOAT_TYPE:
                        *(float*)setting.var_addr = std::stof(value);
                        break;
                    default:
                        std::cerr << "Unsupported type for key: " << key << std::endl;
                        break;
                }
            }
        }
    }
}

void Config::saveConfig(){
    // Lock the semaphore before writing to the file
    sem_wait(&configFileLock);

    // Save the current settings to the config file
    std::ofstream configFile(configFileName);
    if (configFile.is_open()) {
        for (const auto& setting : settings) {
            if (setting.var_addr == nullptr) continue;  // Skip empty slots
            configFile << setting.param << "=";
            switch (setting.var_type) {
                case STRING_TYPE:
                    configFile << (char*)setting.var_addr;
                    break;
                case INT_TYPE:
                    configFile << *(int*)setting.var_addr;
                    break;
                case LONG_TYPE:
                    configFile << *(long*)setting.var_addr;
                    break;
                case FLOAT_TYPE:
                    configFile << *(float*)setting.var_addr;
                    break;
                // Handle other types as needed
            }
            configFile << std::endl;
        }
        configFile.close();
    } else {
        std::cerr << "Failed to save config file!" << std::endl;
    }
    // Unlock the semaphore after writing
    sem_post(&configFileLock);
    logAppConfigurations();
}

void Config::logAppConfigurations(){
    // Lock the semaphore before reading from the file
    sem_wait(&configFileLock);

    std::ifstream configFile(configFileName);

    if(!configFile.is_open()){
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }
    std::cout<<"---------App Configurations----------"<<std::endl;
    
    std::string line;
    while(std::getline(configFile,line)){
        std::cout<<line<<std::endl;
    }   
    configFile.close();
    std::cout<<"-------------------------------------"<<std::endl;
    // Unlock the semaphore after reading
    sem_post(&configFileLock);
}

void Config::addNewConfigurations(){
    // Lock the semaphore before writing to the file
    sem_wait(&configFileLock);

    // Open the file to read its content first
    std::ifstream configFile(configFileName);
    std::vector<std::string> fileLines;

    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            fileLines.push_back(line);  // Store existing lines
        }
        configFile.close();
    }

    // Now open the config file to write or append to it
    std::ofstream outFile(configFileName, std::ios::app);  // Open in append mode
    if (outFile.is_open()) {
        for (const auto& setting : settings) {
            if (setting.var_addr == nullptr) continue;  // Skip empty slots
            bool exists = false;

            // Check if the parameter is already in the file
            for (const auto& line : fileLines) {
                if (line.find(setting.param) != std::string::npos) {
                    exists = true;
                    break;
                }
            }

            // If the parameter doesn't exist, add it with default value
            if (!exists) {
                outFile << setting.param << "=" << setting.def_value << std::endl;
                std::cout << "Added missing parameter: " << setting.param << " with default value " << setting.def_value << std::endl;
            }
        }
        outFile.close();
    } else {
        std::cerr << "Failed to save config file!" << std::endl;
    }

    // Unlock the semaphore after writing
    sem_post(&configFileLock);
}

void Config::updateConfiguration(const char *paramName,const char *newValue){
    int temp_int_val = 0;
    long temp_long_val = 0;
    float temp_float_val = 0.0;
    for(auto& param:settings){
        if(std::string(param.param) == paramName){
            switch (param.var_type) {
                case STRING_TYPE:
                    memcpy(param.var_addr,newValue,param.var_len);
                    break;
                case INT_TYPE:
                    sscanf(newValue,"%d",&temp_int_val);
                    memcpy(param.var_addr,&temp_int_val,param.var_len);
                    break;
                case LONG_TYPE:
                    sscanf(newValue,"%ld",&temp_long_val);
                    memcpy(param.var_addr,&temp_long_val,param.var_len);
                    break;
                case FLOAT_TYPE:
                    sscanf(newValue,"%f",&temp_float_val);
                    memcpy(param.var_addr,&temp_float_val,param.var_len);
                    break;
            }
            saveConfig();
            return;           
        }
    }
}

int Config::getConfigCountInConfigFile(){
    int lineCount=0;
    // Lock the semaphore before reading from the file
    sem_wait(&configFileLock);

    std::ifstream configFile(configFileName);

    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            ++lineCount;
        }
        configFile.close();
    } else {
        std::cerr << "Unable to open file: " << configFileName << std::endl;
    }
    // Unlock the semaphore after reading
    sem_post(&configFileLock);
    return lineCount;
}