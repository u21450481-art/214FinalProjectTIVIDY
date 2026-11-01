/**
 * @file DataPayload.cpp
 * @brief Implementation of the DataPayload class.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - Role: Context Data Carrier / Runtime State Container (Composed by WorkflowInstance)
 * - Ensures TIVIDY remains a generic workflow engine capable of executing two
 *   distinct industry scenarios without modifying core engine classes.
 * ============================================================================
 */

#include "DataPayload.h"
#include "Log.h"

DataPayload::DataPayload() : values() {}

DataPayload::DataPayload(const DataPayload &other) : values(other.values) {}

DataPayload::~DataPayload() {}

void DataPayload::set(const std::string &key, const std::string &value)
{
    values[key] = value;
}

std::string DataPayload::get(const std::string &key) const
{
    std::map<std::string, std::string>::const_iterator it = values.find(key);
    if (it != values.end())
    {
        return it->second;
    }
    return "";
}

bool DataPayload::has(const std::string &key) const
{
    return values.find(key) != values.end();
}

bool DataPayload::matches(const std::string &condition) const
{
    if (condition.empty())
        return true;

    // check != first, because "a!=b" also contains an =
    std::string::size_type pos = condition.find("!=");
    if (pos != std::string::npos)
        return get(condition.substr(0, pos)) != condition.substr(pos + 2);

    pos = condition.find('=');
    if (pos == std::string::npos)
        return true; // not a condition we understand, so don't block the task
    return get(condition.substr(0, pos)) == condition.substr(pos + 1);
}

bool DataPayload::remove(const std::string &key)
{
    return values.erase(key) > 0;
}

void DataPayload::clear()
{
    values.clear();
}

const std::map<std::string, std::string> &DataPayload::getAll() const
{
    return values;
}

void DataPayload::printPayload() const
{
    TLOG("=== DataPayload Contents ===");
    if (values.empty())
    {
        TLOG("  (empty)");
        return;
    }
    for (std::map<std::string, std::string>::const_iterator it = values.begin();
         it != values.end(); ++it)
    {
        TLOG("  [" << it->first << "] = " << it->second);
    }
}
