// rev_audit.h
#pragma once
#include "rev512.h"
#include <string>
#include <vector>
#include <ctime>

namespace rev_audit {

struct Entry {
    std::string input;
    std::string output;
    std::string hash;     // rev512(input + output + timestamp)
    std::time_t timestamp;
};

class AuditTrail {
public:
    void record(const std::string& input, const std::string& output);
    bool verify(size_t index) const;
    std::string export_json() const;
    size_t size() const;
    void clear();
private:
    std::vector<Entry> entries;
    std::string compute_hash(const std::string& in, const std::string& out, std::time_t ts) const;
};

}