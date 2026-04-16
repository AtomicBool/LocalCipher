#include "utils/Contact.h"

// =====================================================
// constructor
// =====================================================
ContactManager::ContactManager(const std::string& filename)
    : filename(filename)
{
    load();
}

// =====================================================
// load
// =====================================================
void ContactManager::load()
{
    std::ifstream file(filename);
    if (!file.is_open())
        return;

    contacts.clear();

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);

        std::string name, key;

        if (std::getline(ss, name, ',') &&
            std::getline(ss, key))
        {
            contacts.push_back({ name, key });
        }
    }
}

// =====================================================
// save
// =====================================================
void ContactManager::save()
{
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open())
        return;

    for (const auto& c : contacts)
    {
        file << c.name << "," << c.public_key << "\n";
    }
}

// =====================================================
// add
// =====================================================
void ContactManager::addContact(const Contact& contact)
{
    for (auto& c : contacts)
    {
        if (c.name == contact.name)
        {
            c.public_key = contact.public_key;
            save();
            return;
        }
    }

    contacts.push_back(contact);
    save();
}

// =====================================================
// search (optimized, no extra vector copy if empty query)
// =====================================================
std::vector<Contact> ContactManager::search(const std::string& query)
{
    if (query.empty())
        return contacts; // OK for now (explicit copy)

    std::vector<Contact> results;
    results.reserve(contacts.size() / 2);

    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(),
        lowerQuery.begin(), ::tolower);

    for (const auto& c : contacts)
    {
        // avoid full string copy unless needed
        const std::string& name = c.name;

        bool match = false;

        for (size_t i = 0; i < name.size(); i++)
        {
            if (std::tolower(name[i]) ==
                std::tolower(lowerQuery[0]))
            {
                // quick reject skip (cheap heuristic)
                if (name.size() >= lowerQuery.size())
                {
                    std::string_view view(name);
                    std::string lowered(name.size(), ' ');

                    for (size_t j = 0; j < name.size(); j++)
                        lowered[j] = std::tolower(name[j]);

                    if (lowered.find(lowerQuery) != std::string::npos)
                        match = true;
                }
                break;
            }
        }

        if (match)
            results.push_back(c);
    }

    return results;
}