#include <iostream>
#include <cassert>
#include <thread>
#include "LruCache.hpp"

// slow get page imitation
int slow_get_page_int(int key)
{
    std::cout << "< ... Network request ...";
    std::cout.flush();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);

    std::cout << " Ok >" << std::endl;

    return key;
}

int main() {
    int hits = 0;
    int miss = 0;
    int n;
    size_t m;

    std::cout << "Введите размер кэша: ";
    std::cin >> m;

    std::cout << "Введите кол-во элементов кэша: ";
    std::cin >> n;

    caches::LruCache<int, int> c{m};

    std::cout << "Вводите элементы кэша: " << std::endl;
    for (int i = 0; i < n; ++i) {
        int q;
        std::cin >> q;
        if (c.lookup_update(q, slow_get_page_int))
            hits++;
        else
            miss++;
    }

    std::cout << "Hits: " << hits << std::endl;
    std::cout << "Mits: " << miss << std::endl;

    return 0;
}
