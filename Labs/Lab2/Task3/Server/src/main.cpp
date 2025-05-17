#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "logger.hpp"
#include <semaphore>
#include <memory>
#include <thread>
#include <map>
#include <cstring>

#define USERNAME_LEN 32
#define COMMAND_LEN 32
#define TEXT_LEN 4096

#define sem_name "sem_for_messanger"
#define shm_name "shm_for_messanger"
#define SIZE 9000

typedef void (*callback)(void *);
/* 
const char *sem_name = "sem_for_messanger";
const char *shm_name = "shm_for_messanger";
const int SIZE = 9000; */



struct Data;
class Server;

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

class Server
{

    static std::map<std::string, std::pair<std::shared_ptr<Data>, std::shared_ptr<Server>>> clients;
    std::vector<std::thread> threads;
    static bool toExit;
    static std::unique_ptr<Logger> logger;
    std::vector<std::pair<std::string, sem_t *>> semaphores;
    void *ptr = nullptr;
    int shm_fd;
    size_t size;
    std::string sharedMemoryName;
    static std::mutex toExitAccess;
    std::mutex forOutput;

public:
    Server() {}

    virtual ~Server()
    {
        for (auto &thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        if (ptr != nullptr)
        {
            munmap(ptr, SIZE);
        }
        shm_unlink(sharedMemoryName.c_str());
        logger->InfoMessage("Server closed.");
        for (size_t i = 0; i < semaphores.size(); i++)
        {
            sem_unlink(semaphores[i].first.c_str());
            sem_close(semaphores[i].second);
            logger->InfoMessage("Semaphore " + semaphores[i].first + " closed.");
        }
    }

    size_t openSemaphore(const char *_sem_name)
    {
        sem_t *sem;
        sem = sem_open(_sem_name, O_CREAT, 0644, 1);
        if (sem == SEM_FAILED)
        {
            logger->CriticalMessage("Semaphore opening error!");
            std::cerr << "sem_open: " << errno << std::endl;
            throw std::exception();
        }
        semaphores.push_back(std::make_pair(std::string(_sem_name), sem));
        logger->InfoMessage("Semaphore was opened successfuly.");
        return semaphores.size() - 1;
    }

    void openSharedMemory(const char *_name)
    {
        sharedMemoryName = std::string(_name);
        shm_fd = shm_open(_name, O_CREAT | O_RDWR, 0666);
        logger->InfoMessage("Shared memory was opened.");
        ftruncate(shm_fd, SIZE);
        return;
    }

    void getPtr(size_t _size)
    {
        size = _size;
        ptr = mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (ptr == MAP_FAILED)
        {
            logger->CriticalMessage("Memory mapping failed");
            std::cerr << "Memory mapping failed\n";
            throw std::exception();
        }
        return;
    }

    void *getOutputPtr()
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

    void answer(Data &data)
    {
        memcpy((char *)ptr + sizeof(Data), &data, sizeof(Data));
        return;
    }

    void *shmAccess() const
    {
        return ptr;
    }

    static bool getBoolToExit()
    {
        toExitAccess.lock();
        bool ret = toExit;
        toExitAccess.unlock();
        return ret;
    }

    static void chatting(Data *&data, std::pair<std::shared_ptr<Data>, std::shared_ptr<Server>> pr)
    {
        auto i = clients.find(std::string(data->userName));
        std::shared_ptr<Server> me = i->second.second;
        if (i == clients.end())
        {
            strcpy(data->text, "No such user online");
            me->answer(*data);
            me->semPost(1);
            logger->ErrorMessage("No such user online " + std::string(data->userName));
            return;
        }

        std::string input;
        char msg[TEXT_LEN];
        msg[0] = '0';
        msg[1] = '\0';
        std::string name1(data->userName), name2(pr.first->userName);
        if (name1 > name2)
        {
            std::swap(name1, name2);
        }

        std::ifstream fileo(name1 + "_" + name2 + ".txt");
        std::string line;
        if (fileo.is_open())
        {

            while (std::getline(fileo, line))
            {
                strcpy(data->text, line.c_str());
                data->text[strlen(data->text)] = '\0';
                me->answer(*data);
                usleep(500);
                me->semPost(1);
            }
            fileo.close();
        }

        command_t t = NO_COMMAND;
        while (t != CLOSE)
        {
            me->semWait(0);

            Data fromClient = *((Data *)me->shmAccess());

            strcpy(msg, fromClient.text);

            if (fromClient.command == CLOSE)
            {
                me->semPost(0);
                me->semWait(0);
                break;
            }
            t = fromClient.command;
            pr.second->semWait(2);

            strcpy(pr.first->text, "[from ");
            strcat(pr.first->text, data->userName);
            strcat(pr.first->text, "]: ");
            strcat(pr.first->text, msg);

            std::ofstream file(name1 + "_" + name2 + ".txt", std::ios::app);

            if (!file.is_open())
            {
                pr.second->semPost(2);
                pr.second->semPost(1);
                me->semPost(0);
                me->semWait(0);
                logger->ErrorMessage("File error");
                return;
            }

            line = pr.first->text;
            file << line << std::endl;

            file.close();

            pr.second->answer(*pr.first);

            pr.second->semPost(2);
            pr.second->semPost(1);
            me->semPost(0);
            me->semWait(0);
        }
        return;
    }

    void messageHandler()
    {
        std::array<command_t, 3> commandsId = {HELP, OPEN, CLOSE};
        std::array<callback, 3> clb = {Server::help, Server::open, Server::close};
        semWait(0);
        while (!getBoolToExit())
        {
            semWait(0);
            forOutput.lock();

            if (getBoolToExit())
            {
                return;
            }

            Data fromClient = *((Data *)shmAccess());
            bool isRightCommand = false;
            for (size_t i = 0; i < commandsId.size(); i++)
            {
                if (commandsId[i] == fromClient.command)
                {
                    isRightCommand = true;
                    if (commandsId[i] == CLOSE)
                    {
                        strcpy(fromClient.text, "You must be in chat to use this command!");
                        break;
                    }
                    clb[i]((void *)&fromClient);
                    break;
                }
            }
            if (isRightCommand == false)
            {
                strcpy(fromClient.text, "");
            }
            answer(fromClient);

            semPost(1);

            semPost(0);
            semWait(0);
            forOutput.unlock();
        }
    }

    static void registerClients(Server &server)
    {

        while (!getBoolToExit())
        {
            server.semWait(0);

            if (getBoolToExit())
            {
                return;
            }

            Data fromClient = *((Data *)server.shmAccess());

            std::shared_ptr<Server> serverForClient = std::make_shared<Server>();
            std::shared_ptr<Data> dataFromClient = std::make_shared<Data>();

            serverForClient->openSemaphore(std::to_string(fromClient.pid).c_str());
            serverForClient->openSharedMemory(std::to_string(fromClient.pid).c_str());
            serverForClient->getPtr(SIZE);

            dataFromClient->command = fromClient.command;
            dataFromClient->pid = fromClient.pid;
            strcpy(dataFromClient->text, fromClient.text);
            strcpy(dataFromClient->userName, fromClient.userName);

            auto pr = std::make_pair(dataFromClient, serverForClient);
            std::string name = std::string(pr.first->userName);
            clients[name] = pr;

            server.threads.push_back(std::thread(Server::makeConnection, std::ref(clients[name])));
            logger->InfoMessage(std::string(fromClient.userName) + " connected to the server.");

            server.semPost(0);
            server.semWait(0);
            server.semPost(1);
        }

        for (size_t i = 0; i < server.threads.size(); i++)
        {
            server.threads[i].join();
        }
    }

    static void makeConnection(std::pair<std::shared_ptr<Data>, std::shared_ptr<Server>> &dt)
    {
        std::string s = std::to_string(dt.first->pid) + "output";
        std::string s1 = std::to_string(dt.first->pid) + "chatting";

        dt.second.get()->openSemaphore(s.c_str());
        dt.second.get()->openSemaphore(s1.c_str());

        dt.second.get()->messageHandler();
    }

    static void pressToExit(Server &server)
    {
        std::string input;
        std::cin >> input;

        toExitAccess.lock();
        toExit = true;
        toExitAccess.unlock();

        for (auto i = clients.begin(); i != clients.end(); i++)
        {
            i->second.second->semPost(0);
        }
        server.semPost(0);
        return;
    }

    static void help(void *arg)
    {
        Data *data = (Data *)arg;

        strcpy(data->text, "* help - find out all commands\n"
                           "* open <name> - open a chat with user with name <name>\n"
                           "* close - close the chat\n"
                           "* exit - to exit the messanger");

        return;
    }

    static void open(void *arg)
    {
        Data *data = (Data *)arg;

        auto i = Server::clients.find(std::string(data->text));
        auto j = Server::clients.find(std::string(data->userName));
        if (i == Server::clients.end())
        {
            strcat(data->text, " is not online!");
            return;
        }
        if (j == Server::clients.end())
        {
            strcat(data->text, "No such user!");
            return;
        }
        else
        {
            auto pr = i->second;
            ;
            j->second.second->threads.push_back(std::thread(Server::chatting, std::ref(data), std::ref(pr)));
            j->second.second->threads.back().join();
        }
        return;
    }

    static void close(void *arg)
    {
        (void)arg;
        return;
    }
};

bool Server::toExit = false;
std::mutex Server::toExitAccess;
std::map<std::string, std::pair<std::shared_ptr<Data>, std::shared_ptr<Server>>> Server::clients;
std::unique_ptr<Logger> Server::logger = UniquePtrLoggerBuilder()
                                     .add_handler(std::cout)
                                     .add_handler(std::make_unique<std::ofstream>("logs.txt"))
                                     .set_level(Logger::INFO)
                                     .make_object();

int main()
{
    Server server;
    server.openSemaphore(sem_name);
    server.openSharedMemory(shm_name);
    server.getPtr(SIZE);
    server.openSemaphore("dop");

    server.semWait(0);

    std::thread regClientsT(Server::registerClients, std::ref(server));
    std::thread ptE(Server::pressToExit, std::ref(server));

    regClientsT.join();
    ptE.join();
    return 0;
}
