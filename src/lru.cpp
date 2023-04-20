#include "lru.h"

LRUCache::LRUCache(size_t max_size) {
    max_size_ = max_size;
    queue_.clear();
    map_.clear();
}

std::string LRUCache::get(std::string key) {
    // Check if it's in the map
    auto it = map_.find(key);
    if (it == map_.end()) {
        return "";
    }

    // Move the value to the front of the queue
    queue_.splice(queue_.begin(), queue_, it->second);

    // Return the value
    return it->second->second;
}

bool LRUCache::set(std::string key, std::string value) {
    // Check if the value is in the map
    auto it = map_.find(key);

    // Key already exists, so just move it to front and update it
    if (it != map_.end()) {
        queue_.splice(queue_.begin(), queue_, it->second);
        it->second->second = value;
        return false; // didn't create a new value, so return false
    }

    // If there's not enough space, eject the last key
    if (map_.size() == max_size_) {
        auto last = queue_.end(); // points to element after last element
        last--; // now it points to last element
        map_.erase(last->first);
        queue_.pop_back();
    }

    // Set the value
    queue_.emplace_front(key, value);
    map_[key] = queue_.begin();

    return true; // created a new thing, so return true
}
