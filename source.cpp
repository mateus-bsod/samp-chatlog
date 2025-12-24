#include "plugin.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>

using namespace plugin;

std::string getdate()
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}


class ChatLog
{
public:
    int initialized = false;

    bool IsMultiplayer()
    {
        return GetModuleHandleA("samp.dll") != nullptr ||
            GetModuleHandleA("omp.dll") != nullptr;
    }

    ~ChatLog()
    {
        if (!instance.IsMultiplayer()) return;

        const char* path = R"(C:\Users\Administrator\Documents\GTA San Andreas User Files\SAMP\chatlog.txt)";
        if (std::filesystem::exists(path))
        {
            std::string hoje = getdate();
            std::ifstream chatlog(path);
            std::ofstream new_chatlog(("C:\\Users\\Administrator\\Documents\\GTA San Andreas User Files\\SAMP\\ChatLogs\\chatlog " + hoje + ".txt"), std::ios::app);

            if (!chatlog.is_open() || !new_chatlog.is_open())
                return;

            std::string line;
            while (std::getline(chatlog, line))
            {
                if (line.size() >= 11 && line[0] == '[' && line[9] == ']')
                {
                    std::string time = line.substr(1, 8);
                    std::string rest = line.substr(9);

                    new_chatlog << "[" << hoje << " " << time << "]" << rest.substr(1) << "\n";
                }
            }

            new_chatlog.close();
            chatlog.close();
        }
    }


    ChatLog()
    {
        Events::initScriptsEvent += []
        {
            instance.initialized = false;
        };

        Events::processScriptsEvent += []
        {
            if (!instance.initialized)
            {
                instance.initialized = true;
            }
            else
            {
                if (instance.IsMultiplayer())
                {
                    const char* path = R"(C:\Users\Administrator\Documents\GTA San Andreas User Files\SAMP\ChatLogs)";
                    if (!std::filesystem::exists(path)) std::filesystem::create_directory(path);
                }
            }
        }; 
    }
} instance;