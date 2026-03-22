#ifndef LFUCACHE_H
#define LFUCACHE_H

#include <cstddef>
#include <list>
#include <unordered_map>

/*
Этот код реализует обобщённый (template) LRU-кэш (Least Recently Used Cache) — структуру данных,
которая хранит ограниченное количество элементов и автоматически удаляет наименее недавно использованные.

При доступе к элементу → он становится "самым свежим"
При переполнении → удаляется самый старый (давно не использованный)

*/

namespace caches {

template <typename T, typename KeyT>
class LruCache
{
private:
    // определяет сколько элементов в кэше может быть сохранено
    size_t size_;

    // двусвязный список из пар ключ-значение (сами элементы кэша)
    // хранит элементы в порядке от самого свежего (begin) к самому старому (end)
    std::list<std::pair<KeyT, T>> cache_;

    // тип итератора по элементам кэша
    using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;

    // хэш-таблица - хранит итераторы на элементы двусвязного списка
    // по ключу быстро даёт итератор на элемент в списке
    std::unordered_map<KeyT, ListIt> hash_;

public:
    // тут задается максимальный размер кэша
    LruCache(size_t sz = 10) : size_(sz) {} // ctor

    // метод проверяет заполнен ли кэш
    bool full() const { return (cache_.size() == size_); }

    // KeyT key - ключ (id-ресурса, который хотим запросить)
    // F slow_get_page - медленная функция, которая может вернуть этот ресурс по сети
    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page);
};

template <typename T, typename KeyT>
template <typename F>
bool LruCache<T, KeyT>::lookup_update(KeyT key, F slow_get_page) {
    // проверяем есть ли такой id кэше локально
    auto hit = hash_.find(key);
    // если нету в кэше локально (всё прошли и на последнем итераторе остановились)
    if (hit == hash_.end()) {
        // если кэш заполнен
        if (full()) {
            // Удаляем самый старый элемент (least recently used)
            hash_.erase(cache_.back().first);   // из хэш-таблицы
            cache_.pop_back();                  // из списка
        }
        // Добавляем в начало списка (самый свежий)
        cache_.emplace_front(key, slow_get_page(key));
        // Сохраняем итератор в хэш-таблице
        hash_.emplace(key, cache_.begin());
        return false; // miss - элемент не был найден в кэше
    }

    auto eltit = hit->second;                       // итератор на элемент в списке
    cache_.splice(cache_.begin(), cache_, eltit);   // переместить в начало (освежить элемент в кэше)
    return true; // hit - элемент был найден в кэше
}

} // namespace caches

#endif // LFUCACHE_H
