#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

struct Contact {
    std::string name;
    std::string public_key;
};

class ContactManager {
public:
    ContactManager(const std::string& filename);
    
    void load();
    void save();
    void addContact(const Contact& contact);
    std::vector<Contact> search(const std::string& query);
    const std::vector<Contact>& getAllContacts() const { return contacts; }

private:
    std::string filename;
    std::vector<Contact> contacts;
};