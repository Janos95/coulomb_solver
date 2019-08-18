//
// Created by janos on 18.08.19.
//

#include "struct_of_arrays.hpp"

#include <fmt/core.h>

#include <mpi.h>

//for read
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>
#include <algorithm>
#include <string_view>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <optional>
#include <numeric>
#include <fstream>
#include <random>



template<typename T>
inline bool parseReal(const char* str, char** str_end, T& value)
{
    //TODO: check for HUGE_VAL, HUGE_VALF or HUGE_VALL

    if constexpr(std::is_same_v<T, float>)
        value = strtof(str, str_end);
    else if constexpr(std::is_same_v<T, double>)
        value = strtod(str, str_end);
    else if constexpr(std::is_same_v<T, long double>)
        value = strtold(str, str_end);

    return str != *str_end;
}

template<typename T>
bool parseLine(std::string_view& view, std::array<T, 4>& v)
{
    const char *b = view.data();
    char *e;

    for (int i = 0; i < 4; ++i)
    {
        if(!parseReal<T>(b, &e, v[i]))
            return false;
        b = e;
    }

    view.remove_prefix(e - view.data());
    return true;
}


auto divideIntoChunks(std::string_view view, int numChunks)
{
    std::vector<std::string_view> chunks;
    auto chunkSize = view.size() / numChunks; // in worst case overschedule a bit

    while(chunkSize < view.size())
    {
        auto it = std::find(view.begin() + chunkSize, view.end(), '\n'); assert(it != view.end());
        auto n = std::distance(view.begin(), it);
        chunks.push_back(view.substr(0,n));
        view.remove_prefix(n + 1);
    }

    if(!view.empty())
        chunks.push_back(view);

    return chunks;
}

template<typename T>
class ThreadedParser
{
public:

    explicit ThreadedParser(std::string_view file): m_file(file) {}

    Points4<T> parse(int numThreads)
    {
        fmt::print("Using {} worker threads to parse text file\n", numThreads);

        Points4<T> points;

        auto chunks = divideIntoChunks(m_file, numThreads);
        std::vector<std::thread> workers;
        for(int i = 0; i < chunks.size(); ++i)
            workers.emplace_back([this, i, &points, &chunks](){ return load(chunks[i], i, points);});
        for(auto& worker: workers) //hit'em hard
            worker.join();

        return points;
    }

private:

    void load(std::string_view chunk, int index, Points4<T>& points)
    {
        Points4<T> pointsInChunk;
        std::array<T, 4> v;
        while (!chunk.empty() && parseLine(chunk, v))
            pointsInChunk.push_back(v[0], v[1], v[2], v[3]);


        std::unique_lock lock(mutex);
        cv.wait(lock, [&]{return index == count;});
        points.insert(points.end(), pointsInChunk.begin(), pointsInChunk.end());
        ++count;
        cv.notify_all();
    }


    std::string_view m_file;

    std::mutex mutex; //protect count and points
    std::condition_variable cv;
    int count = 0;
};


auto handleError(std::string_view error)
{
    fmt::print("Error in {}\n", error);
    std::exit(255);
}



template<typename T>
Points4<T> readAndParseFile(const std::string& filename)
{

    auto fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1)
        handleError("open");

    struct stat status;
    if (fstat(fd, &status) == -1)
        handleError("fstat");

    auto size = status.st_size;
    auto data = std::make_unique<char[]>(size);

    posix_fadvise(fd, 0, 0, 1); //sequential advice

    auto readStatus = read(fd, data.get(), size);
    if(readStatus == -1)
        handleError("read");

    close(fd);

    ThreadedParser<T> parser(std::string_view(data.get(), size));
    return parser.parse(std::thread::hardware_concurrency());
}


template<typename T>
void writePointsToFile(const std::string& filename, const Points4<T>& points)
{
    int r, s;
    MPI_Comm_size(MPI_COMM_WORLD, &s);
    MPI_Comm_rank(MPI_COMM_WORLD, &r);



    MPI_File fh;
    MPI_File_open(
            MPI_COMM_WORLD,
            filename.c_str(),
            MPI_MODE_WRONLY | MPI_MODE_CREATE,
            MPI_INFO_NULL,
            &fh);

    MPI_Datatype type;

    int numPoints = points.size();
    int begin = r * (numPoints / s + 1);
    int chunkSize = r == (s - 1) ? numPoints - begin : (numPoints / s + 1);
    int end = begin + chunkSize;
    assert(end < size);

    std::string partialString;

    auto [itAx, itAy, itAz, itPot] = points.begin();
    for (int j = begin; j < end; ++j)
        partialString += fmt::format("{} {} {} {}\n", *(itAx + j), *(itAy + j), *(itAz + j), *(itPot + j));

    std::vector<int> bufferSizes(s), displacements(s + 1, 0);
    int bufferSize = partialString.size();

    MPI_Allgather(
            &bufferSize,
            1,
            MPI_INT,
            bufferSizes.data(),
            1,
            MPI_INT,
            MPI_COMM_WORLD);

    std::partial_sum(bufferSizes.begin(), bufferSizes.end(), displacements.begin() + 1);

    MPI_File_set_view(
            fh,
            displacements[r],
            MPI_CHAR,
            MPI_CHAR,
            "native",
            MPI_INFO_NULL);

    MPI_File_write(fh, partialString.c_str(), bufferSize, MPI_CHAR, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);
}


template<typename T>
void generateTestFile (const int length, const std::string& filename)
{
    std::random_device rd;
    std::default_random_engine e(rd());
    std::uniform_real_distribution<T> dist(-1, 1);

    std::ofstream file(filename);
    for (int i = 0; i < length; ++i)
    {
        file << fmt::format("{} {} {} {}\n", dist(e), dist(e), dist(e), dist(e));
    }
}



