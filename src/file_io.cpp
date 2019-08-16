// Created by janos on 8/5/19.
//



#include "vector.hpp"

#include <string>
#include <thread>
#include <mutex>


#include <fmt/core.h>

#include <vector>

#include <algorithm>
#include <cstring>


//for read
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void handle_error(const char* msg) {
    perror(msg);
    exit(255);
}

Vector4f parseLine(const char* b, char*& e)
{
    Vector4f v;
    auto b1 = b;

    for (int i = 0; i < 4; ++i)
    {
        v[i] = strtof(b1, &e);
        b1 = e;
    }

    return v;
}


struct Chunk
{
    Chunk(const char* begin, const char* newLine): begin(begin), newLine(newLine){}
    const char* begin; //one character after new line
    const char* newLine;
};

template<typename RandomIt>
auto divideIntoChunks(RandomIt first, RandomIt last, int chunkSize)
{
    std::vector<Chunk> chunks;
    while(last - first > chunkSize)
    {
        auto chunkBegin = first;
        first = std::find(first + chunkSize, last, '\n'); //TODO: EOF instead of newline?
        chunks.emplace_back(chunkBegin, first);
        first += 1;
    }

    assert(end - begin >= 0 );

    if(last != first)
        chunks.push_back(Chunk{first, last}); //last chunk

    return chunks;
}

struct parseChunk
{

    Chunk chunk;
    std::vector<Vector4f>& points;
    std::mutex& mutex; //always use protection

    void operator()()
    {
        std::vector<Vector4f> pointsInChunk;

        auto [begin, end] = chunk;
        char* newLine;

        while (begin != end)
        {
            pointsInChunk.push_back(parseLine(begin, newLine));
            if(!newLine || *newLine != '\n')
                handle_error("file formatting not valid");
            else if(newLine == end)
                break;
            begin = newLine + 1;
        }

        std::lock_guard lock(mutex);
        points.insert(points.end(), pointsInChunk.begin(), pointsInChunk.end());
    }
};

class MemoryMappedFile
{
public:
    explicit MemoryMappedFile(const std::string& filename)
    {
        auto m_fd = open(filename.c_str(), O_RDONLY);
        if (m_fd == -1)
            handle_error("open");

        struct stat status;
        if (fstat(m_fd, &status) == -1)
            handle_error("fstat");

        m_size = status.st_size;
        m_data = std::make_unique<char[]>(m_size);

        posix_fadvise(m_fd, 0, 0, 1); //sequential advice

        auto readStatus = read(m_fd, m_data.get(), this -> size());
        if(readStatus == -1)
            handle_error("read");

        close(m_fd);
    }

    const char* begin()
    {
        return m_data.get();
    }

    const char* end()
    {
        static auto end = m_data.get() + this -> size();
        return end;
    }

    int size()
    {
        return m_size;
    }


private:

    int m_size;
    std::unique_ptr<char[]> m_data;

};

std::vector<Vector4f> loadFile(const std::string& filename)
{

    std::vector<Vector4f> points;
    std::mutex mutex;

    MemoryMappedFile mappedFile(filename);

    fmt::print("file size {}\n", mappedFile.end() - mappedFile.begin());

    auto numThreads = std::thread::hardware_concurrency();
    auto chunks = divideIntoChunks(mappedFile.begin(), mappedFile.end(), mappedFile.size() / numThreads);

    fmt::print("Using {} worker threads to load text file\n", chunks.size());

    std::vector<std::thread> workers;
    for(const auto& chunk: chunks)
        workers.emplace_back(parseChunk{chunk, points, mutex});
    for(auto& worker: workers) //hit'em hard
        worker.join();

    return points;
}



