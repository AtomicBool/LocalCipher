#include "utils/Contact.h"
#include <sstream>

ContactManager::ContactManager(const std::string& filename) : filename(filename) {
    load();
}

void ContactManager::load() {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    contacts.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name, key;
        if (std::getline(ss, name, ',') && std::getline(ss, key)) {
            contacts.push_back({ name, key });
        }
    }
}

void ContactManager::save() {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& contact : contacts) {
        file << contact.name << "," << contact.public_key << "\n";
    }
}

void ContactManager::addContact(const Contact& contact) {
    contacts.push_back(contact);
    save();
}

std::vector<Contact> ContactManager::search(const std::string& query) {
    if (query.empty()) return contacts;

    std::vector<Contact> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& contact : contacts) {
        std::string lowerName = contact.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(contact);
        }
    }
    return results;
}