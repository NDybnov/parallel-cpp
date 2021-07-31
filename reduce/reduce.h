#pragma once

#include <atomic>
#include <cstddef>
#include <algorithm>
#include <thread>
#include <numeric>
#include <vector>

template <typename RandomAccessIterator, class T, class Func>
T parallel_reduce(RandomAccessIterator begin, RandomAccessIterator end,
                  const T& initial_value, Func func, size_t n_threads) {
    const size_t step = (end - begin) / n_threads;
    std::vector<std::thread> threads;
    std::vector<T> res(n_threads);
    for (size_t i = 0; i < n_threads; ++i) {
        RandomAccessIterator first = begin + step * i;
        RandomAccessIterator last = (i != n_threads - 1) ? begin + step * (i + 1) : end;
        threads.emplace_back([first, last, &func, i, initial_value, &res] () {
            res[i] = std::reduce(first, last, initial_value, func);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    T ans = std::reduce(res.begin(), res.end(), initial_value, func);
    return ans;
}
