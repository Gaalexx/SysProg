#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <algorithm>
#include <thread>
#include <list>
#include <cstring>
#include "logger.hpp"

#define USERNAME_LEN 32
#define COMMAND_LEN 32
#define TEXT_LEN 4096

#define sem_name "sem_for_messanger"
#define shm_name "shm_for_messanger"
#define SIZE 9000

typedef int (*callback)(void *);

/* const char *sem_name = "sem_for_messanger";
const char *shm_name = "shm_for_messanger";
const int SIZE = 9000; */

enum command_t
{
    HELP,
    OPEN,
    SEND,
    CLOSE,
    EXIT,
    NO_COMMAND,
    UNKNOWN,
    SERVER_ANSWER
};

struct Data
{
    pid_t pid;
    char userName[USERNAME_LEN];
    command_t command;
    char text[TEXT_LEN];
};

class Client
{
    std::vector<std::pair<std::string, sem_t *>> semaphores;
    static bool toExit;
    static bool inChat;
    static std::mutex toExitAccess;
    static std::unique_ptr<Logger> logger;
    static std::mutex forSemaphoresVector;
    void *ptr = nullptr;
    int shm_fd;
    size_t size;
    std::string sharedMemoryName;

public:
    Client() = default;

    virtual ~Client()
    {
        if (ptr != nullptr)
        {
            munmap(ptr, size);
            close(shm_fd);
        }
        shm_unlink(sharedMemoryName.c_str());
        for (size_t i = 0; i < semaphores.size(); i++)
        {
            sem_close(semaphores[i].second);
            sem_unlink(semaphores[i].first.c_str());
        }
    }

    int openSemaphore(const char *_sem_name)
    {
        std::string semaphoreName = std::string(_sem_name);
        sem_t *sem = sem_open(_sem_name, 0);
        if (sem == SEM_FAILED)
        {
            logger->ErrorMessage("Semaphore opening error!");
            return 0;
        }
        semaphores.push_back(std::make_pair(semaphoreName, sem));
        logger->InfoMessage("Semaphore was opened successfuly.");
        return 1;
    }

    void openSharedMemory(const char *_name)
    {
        sharedMemoryName = std::string(_name);
        shm_fd = shm_open(_name, O_RDWR, 0666);
        if (shm_fd == -1)
        {
            logger->ErrorMessage("Failed to open shared memory.");
            throw std::exception();
        }
        logger->InfoMessage("Shared memory was opened.");
        return;
    }

    void getPtr(size_t _size)
    {
        size = _size;
        ptr = mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (ptr == MAP_FAILED)
        {
            logger->CriticalMessage("Memory mapping failed");
            close(shm_fd);
            throw std::exception();
        }
        logger->InfoMessage("Memory mapping done.");
        return;
    }

    static bool getBoolToExit()
    {
        toExitAccess.lock();
        bool ret = toExit;
        toExitAccess.unlock();
        return ret;
    }

    void *getInputPtr()
    {
        if (ptr == nullptr)
        {
            return nullptr;
        }
        return (void *)((char *)ptr + sizeof(Data));
    }

    size_t getSize() const
    {
        return size;
    }

    const sem_t *getSem(const size_t &index) const
    {
        if (semaphores.size() <= index)
        {
            return nullptr;
        }
        return semaphores[index].second;
    }

    int semValue(const size_t &index) const
    {
        if (semaphores.size() <= index)
        {
            return -1;
        }
        int sv;
        sem_getvalue(semaphores[index].second, &sv);
        return sv;
    }

    int semWait(const size_t &index)
    {
        if (semaphores.size() <= index)
        {
            return -1;
        }
        return sem_wait(semaphores[index].second);
    }

    int semTryWait(const size_t &index)
    {
        if (semaphores.size() <= index)
        {
            return -1;
        }
        return sem_trywait(semaphores[index].second);
    }

    int semPost(const size_t &index)
    {
        if (semaphores.size() <= index)
        {
            return -1;
        }
        return sem_post(semaphores[index].second);
    }

    void clientPost(Data &data)
    {
        memcpy(ptr, &data, sizeof(Data));
        return;
    }

    void *shmAccess() const
    {
        return ptr;
    }

    static std::pair<command_t, std::string> getCommandAndText(const std::string &text)
    {
        std::array<std::string, 5> commands = {"help", "open", "send", "close", "exit"};
        std::array<command_t, 5> commandsId = {HELP, OPEN, SEND, CLOSE, EXIT};
        auto fnd = std::find(text.begin(), text.end(), ' ');
        if (fnd != text.begin() && fnd != text.end())
        {
            std::string command = std::string(text.begin(), fnd);

            for (size_t i = 0; i < commands.size(); i++)
            {
                if (commands[i] == command)
                {
                    auto pr = std::make_pair(commandsId[i], std::string(fnd + 1, text.end()));
                    return pr;
                }
            }
            auto pr = std::make_pair(UNKNOWN, std::string(fnd + 1, text.end()));
            return pr;
        }
        else
        {
            for (size_t i = 0; i < commands.size(); i++)
            {
                if (commands[i] == text)
                {
                    auto pr = std::make_pair(commandsId[i], " ");
                    return pr;
                }
            }
        }
        return {UNKNOWN, text};
    }

    static void commandHandler(Client &client, Data &data)
    {

        std::string input;

        std::array<std::string, 5> commands = {"help", "open", "send", "close", "exit"};
        char msg[TEXT_LEN];
        bool inChat = false;
        while (input != "exit")
        {
            std::cin.getline(msg, TEXT_LEN);
            client.semWait(2);
            input = msg;
            auto pr = getCommandAndText(input);
            if (!inChat && pr.first == UNKNOWN)
            {
                std::cout << "Unknown command. Type 'help' to find out commands..." << std::endl;
                client.semPost(2);
                continue;
            }
            else
            {
                data.command = pr.first;
                if (data.command == OPEN)
                {
                    inChat = true;
                }
                else if (data.command == CLOSE)
                {
                    inChat = false;
                }
            }
            strcpy(data.text, pr.second.c_str());

            client.clientPost(data);

            client.semPost(0);
            client.semPost(2);
        }
        toExitAccess.lock();
        toExit = true;
        toExitAccess.unlock();
        client.semPost(1);
        return;
    }

    static void output(Client &client, Data &data)
    {
        std::cout << "Hi, " << data.userName << "! Welcome to the messanger!" << std::endl;

        void *oput = client.getInputPtr();
        if (oput == nullptr)
        {
            throw std::exception();
        }

        client.semWait(1);
        while (!getBoolToExit())
        {
            client.semWait(1);

            if (getBoolToExit())
            {
                std::cout << "Good bye! <Exit>" << std::endl;
                return;
            }

            Data fromServer = *((Data *)oput);

            std::cout << fromServer.text << std::endl;

            client.semPost(1);
            client.semWait(1);
        }

        return;
    }
};

bool Client::toExit = false;
std::mutex Client::toExitAccess;
bool Client::inChat = false;
std::unique_ptr<Logger> Client::logger = UniquePtrLoggerBuilder()
                                     .add_handler(std::make_unique<std::ofstream>("logs.txt"))
                                     .set_level(Logger::INFO)
                                     .make_object();

std::mutex Client::forSemaphoresVector;

int main()
{
    Client startCon;

    startCon.openSemaphore(sem_name);
    startCon.openSharedMemory(shm_name);
    startCon.getPtr(SIZE);

    Data data;
    data.pid = getpid();
    if (data.pid == -1)
    {
        std::cerr << "Failed to get pid: " << errno << std::endl;
        return 1;
    }

    while (true)
    {
        std::cout << "Input username: ";
        std::cin.getline(data.userName, USERNAME_LEN);

        for (size_t i = 0; i < strlen(data.userName); i++)
        {
            if (data.userName[i] == '_')
            {
                std::cout << "There must not be _ char in the name!" << std::endl;
                continue;
            }
        }
        break;
    }

    data.command = NO_COMMAND;
    strcpy(data.text, "Nothing");

    startCon.openSemaphore("dop");

    startCon.semWait(1);

    startCon.clientPost(data);

    startCon.semPost(0);

    startCon.semWait(1);

    Client client;
    while (client.openSemaphore(std::to_string(data.pid).c_str()) != 1)
    {
        sleep(1);
    }
    client.openSharedMemory(std::to_string(data.pid).c_str());
    client.getPtr(SIZE);

    startCon.semPost(1);

    std::string sout_name = std::string(std::to_string(data.pid)) + "output";
    while (client.openSemaphore(sout_name.c_str()) != 1)
    {
        sleep(1);
    }

    std::string schat_name = std::string(std::to_string(data.pid)) + "chatting";
    while (client.openSemaphore(schat_name.c_str()) != 1)
    {
        sleep(1);
    }

    std::thread commandHandlerThread(Client::commandHandler, std::ref(client), std::ref(data));
    std::thread outputThread(Client::output, std::ref(client), std::ref(data));

    commandHandlerThread.join();
    outputThread.join();

    return 0;
}
