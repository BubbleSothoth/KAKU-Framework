#pragma once

#define __DLL_EXPORTS__
#ifdef __DLL_EXPORTS__
#define DLLAPI  __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

#include <utility>
#include <mutex>
#include "AbstractDataStructureInterface.h"

template <typename TYPE>
class Score
{
private:
    std::pair<TYPE, std::mutex> score;
public:
    Score(void) = default;
    Score(TYPE val) {
        score.first = val;
        score.second = std::mutex();
    }
    Score(TYPE& object, std::mutex mtx) { score = std::pair<TYPE, std::mutex>(object, mtx); }
    ~Score(void) = default;
    TYPE& GetValue(void) { return score.first; }
    std::mutex& GetMutex(void) { return score.second; };
    void lock(void) { score.second.lock(); }
    void unlock(void) { score.second.unlock(); };
    void AlterValue(TYPE val) {
        std::unique_lock<std::mutex> lock(score.second);
        score.first = val;
        return;
    }
};

