#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

class LoggerBuilder;

class Logger
{
private:
    friend class LoggerBuilder;
    friend class UniquePtrLoggerBuilder;

    std::vector<std::reference_wrapper<std::ostream>> handlers;
    std::vector<std::unique_ptr<std::ostream>> own_handlers;
    unsigned int log_level;

    Logger() {}

    void PrintMessage(const std::string &str, unsigned int lvl = WARNING)
    {
        auto now = std::chrono::system_clock::now();
        auto cur_time = std::chrono::system_clock::to_time_t(now);

        std::string lvl_str;
        switch (lvl)
        {
        case CRITICAL:
            lvl_str = "CRITICAL";
            break;
        case ERROR:
            lvl_str = "ERROR";
            break;
        case WARNING:
            lvl_str = "WARNING";
            break;
        case INFO:
            lvl_str = "INFO";
            break;
        case DEBUG:
            lvl_str = "DEBUG";
            break;
        default:
            return;
        }

        if (log_level <= lvl)
        {
            for (auto &iter : this->handlers)
            {
                iter.get() << std::put_time(std::localtime(&cur_time), "%Y-%m-%d %H:%M:%S") << ": " << lvl_str << ": " << str << std::endl;
            }
        }
    }

public:
    enum LogLevel
    {
        CRITICAL,
        ERROR,
        WARNING,
        INFO,
        DEBUG
    };

    void ChangeLogLevel(LogLevel lvl)
    {
        if (lvl < 0 || lvl > 4)
        {
            throw std::exception();
        }
        log_level = lvl;
    }

    void CriticalMessage(const std::string &str)
    {
        PrintMessage(str, CRITICAL);
    }

    void ErrorMessage(const std::string &str)
    {
        PrintMessage(str, ERROR);
    }

    void WarningMessage(const std::string &str)
    {
        PrintMessage(str, WARNING);
    }

    void InfoMessage(const std::string &str)
    {
        PrintMessage(str, INFO);
    }

    void DebugMessage(const std::string &str)
    {
        PrintMessage(str, DEBUG);
    }
};

class LoggerBuilder
{
private:
    Logger *logger = nullptr;

public:
    LoggerBuilder() { this->logger = new Logger(); }

    LoggerBuilder &set_level(unsigned int logging_level)
    {
        logger->log_level = logging_level;
        return *this;
    }

    LoggerBuilder &add_handler(std::ostream &stream)
    {
        this->logger->handlers.push_back(std::ref(stream));
        return *this;
    }

    LoggerBuilder &add_handler(std::unique_ptr<std::ostream> stream)
    {
        this->logger->handlers.push_back(std::ref(*stream.get()));
        this->logger->own_handlers.push_back(std::move(stream));
        return *this;
    }

    Logger *make_object() { return this->logger; }
};

class UniquePtrLoggerBuilder
{
private:
    std::unique_ptr<Logger> logger = nullptr;

public:
    UniquePtrLoggerBuilder() : logger(new Logger()) {}

    UniquePtrLoggerBuilder &set_level(unsigned int logging_level)
    {
        logger->log_level = logging_level;
        return *this;
    }

    UniquePtrLoggerBuilder &add_handler(std::ostream &stream)
    {
        this->logger->handlers.push_back(std::ref(stream));
        return *this;
    }

    UniquePtrLoggerBuilder &add_handler(std::unique_ptr<std::ostream> stream)
    {
        this->logger->handlers.push_back(std::ref(*stream.get()));
        this->logger->own_handlers.push_back(std::move(stream));
        return *this;
    }

    std::unique_ptr<Logger> make_object() { return std::move(logger); }
};