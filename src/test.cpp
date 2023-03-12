#include <iostream>
#include <map>

int main() {
    std::map<char, float> map1 {{'a', 1.0}, {'b', 2.0}, {'c', 3.0}};
    std::map<char, float> map2 {{'a', 2.0}, {'b', 3.0}, {'c', 4.0}};
    std::map<char, float> map3 = map1; // Create a copy of map1

    // Update the values in map1
    for (auto it = map1.begin(); it != map1.end(); ++it) {
        auto it2 = map2.find(it->first);
        if (it2 != map2.end()) {
            it->second += it2->second;
        }
    }

    // Print the updated values in map1
    std::cout << "Map1:\n";
    for (const auto& pair : map1) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    // Print the values in map3
    std::cout << "\nMap3:\n";
    for (const auto& pair : map3) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    return 0;
}
