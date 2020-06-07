#ifndef CONFIG_REMOTE_H
    #define CONFIG_REMOTE_H

    class ConfigRemote
    {
        uint16_t _status;

        public:
            void Read(const char *config_key, const char *config_url);
            uint16_t GetStatus();
    };

    extern ConfigRemote config_remote;
#endif
