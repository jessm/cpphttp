#include <unordered_map>
#include <list>
#include <string>

class LRUCache {
public:
    LRUCache(size_t max_size = 100);
    std::string get(std::string key);
    bool set(std::string key, std::string value);
private:
    std::list<std::pair<std::string, std::string>> queue_;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> map_;
    size_t max_size_;
};
