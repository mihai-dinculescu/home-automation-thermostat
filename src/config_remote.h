#ifndef CONFIG_REMOTE_H
    #define CONFIG_REMOTE_H

    class ConfigRemote
    {
        uint16_t _field1;

        public:
            void Read(const char *config_url);
            uint16_t GetField1();
    };

    extern ConfigRemote config_remote;
#endif
