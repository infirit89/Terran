#include "trpch.h"
#include "Core/FileUtils.h"
#include "Project/Project.h"

#ifdef TR_LINUX

#include <pthread.h>
#include <sys/inotify.h>
#include <queue>
#include <sys/epoll.h>
#include <sys/eventfd.h>

namespace TerranEngine
{
    std::filesystem::path FileSystem::OpenFile(const char* filter) 
    {
        FILE* file = popen("zenity --file-selection", "r");       
        return "";
    }

    std::filesystem::path FileSystem::SaveFile(const char* filter) 
    {
        FILE* file = popen("zenity --file-selection --save", "r");
        return "";
    }

    void FileSystem::RevealInExplorer(const std::filesystem::path& path) 
    {
        // TODO:
    }


    #define MAX_EVENT_SIZE 20

    static pthread_t s_FileWatchThread;
    static std::atomic<bool> s_Watching = false;
    static int s_EpollFd = 0;
    static int s_WakeUpEventFd = 0;

    static std::unordered_map<int, std::filesystem::path> s_WatchedPaths;

    // TODO: make a file system watcher class
    void FileSystem::StartWatch() 
    {
        s_Watching = true;
        s_EpollFd = epoll_create1(EPOLL_CLOEXEC);
        int ret = pthread_create(&s_FileWatchThread, nullptr, Watch, nullptr);
    }

    void FileSystem::StopWatch()
    {
        if(!s_Watching) return;

        s_Watching = false;
        uint64_t temp = 1;
        write(s_WakeUpEventFd, &temp, sizeof(uint64_t));
        pthread_join(s_FileWatchThread, nullptr);
    }

    static void AddToWatchList(int inotifyFd, std::filesystem::path path) 
    {
        int wd = inotify_add_watch(inotifyFd, path.string().c_str(), IN_DELETE | IN_CREATE | IN_MODIFY | IN_MOVE);

        if(path == Project::GetAssetPath())
            s_WatchedPaths.emplace(wd, "");
        else
            s_WatchedPaths.emplace(wd, std::filesystem::relative(path, Project::GetAssetPath()));
    }

    static void FillWatchList(int inotifyFd, std::filesystem::path path) 
    {   
        std::queue<std::filesystem::path> directories;
        directories.push(path);

        while (!directories.empty())
        {
            std::filesystem::path directoryPath = directories.front();
            AddToWatchList(inotifyFd, directoryPath);
            directories.pop();

            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) 
            {
                if(entry.is_directory())
                    directories.push(entry);
            }
        }
        
    }

    void* FileSystem::Watch(void* params) 
    {
        std::string dirStr = s_PathToWatch.string();
        int fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
        TR_ASSERT(fd > 0, "Couldn't initialize inotify");

        FillWatchList(fd, s_PathToWatch);
        
        epoll_event file_watch_event_desc;
        file_watch_event_desc.events = EPOLLIN;
        file_watch_event_desc.data.fd = fd;
        int ret = epoll_ctl(s_EpollFd, EPOLL_CTL_ADD, fd, &file_watch_event_desc);

        s_WakeUpEventFd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        epoll_event wake_up_event;
        wake_up_event.events = EPOLLIN;
        wake_up_event.data.fd = s_WakeUpEventFd;
        epoll_ctl(s_EpollFd, EPOLL_CTL_ADD, s_WakeUpEventFd, &wake_up_event);

        std::vector<FileSystemChangeEvent> fileChanges;
        constexpr size_t maxFileInformationBufferSize = 20;
        inotify_event buf[maxFileInformationBufferSize];
        inotify_event* eNotify;
        constexpr size_t maxEpollEvents = 10;
        epoll_event epoll_events[maxEpollEvents];
        while(s_Watching)
        {
            int eventCount = epoll_wait(s_EpollFd, &epoll_events[0], maxEpollEvents, 10000);
            if(eventCount <= 0)
                continue;

            for (size_t i = 0; i < eventCount; i++)
            {
                if(epoll_events[i].data.fd == s_WakeUpEventFd)
                    break;
                
                const ssize_t length = read(fd, buf, maxFileInformationBufferSize * sizeof(buf));
                int offset = 0;

                int cookie = 0;
                std::filesystem::path oldFileName = "";
                while(offset < length)
                {
                    FileSystemChangeEvent e;
                    eNotify = (inotify_event*)((char*)buf + offset);
                    std::filesystem::path fileName = s_WatchedPaths[eNotify->wd] / std::filesystem::path(eNotify->name);
                    e.FileName = fileName;
                    offset += sizeof(inotify_event) + eNotify->len;

                    switch(eNotify->mask)
                    {
                    case IN_DELETE: 
                    {
                        e.Action = FileAction::Removed;
                        break;
                    }
                    case IN_CREATE: 
                    {
                        e.Action = FileAction::Added;
                        break;
                    }
                    case IN_MODIFY: 
                    {
                        e.Action = FileAction::Modified;
                        break;
                    }
                    case IN_MOVED_FROM: 
                    {
                        inotify_event* eNotifyNext = (inotify_event*)((char*)buf + offset);
                        if(eNotifyNext->mask == IN_MOVED_TO) 
                        {
                            cookie = eNotify->cookie;
                            oldFileName = fileName;
                        }
                        else
                            e.Action = FileAction::Removed;
                        break;
                    }
                    case IN_MOVED_TO: 
                    {
                        if(cookie != 0) 
                        {
                            e.Action = FileAction::Renamed;
                            e.OldFileName = oldFileName;
                        }
                        else
                            e.Action = FileAction::Added;
                        break;
                    }
                    }

                    if(e.Action != FileAction::None) 
                    {
                        fileChanges.push_back(e);
                    }
                }

                s_ChangeCallback(fileChanges);
                fileChanges.clear();
            }           
        }

        for(const auto& [wd, path] : s_WatchedPaths)
            inotify_rm_watch(fd, wd);

        close(fd);
        close(s_WakeUpEventFd);
        close(s_EpollFd);

        return 0;
    }
}

#endif
