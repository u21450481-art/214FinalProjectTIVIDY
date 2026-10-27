/**
 * @file DataPayload.h
 * @brief Context Data Carrier for TIVIDY Workflow Instances.
 *
 * ============================================================================
 * DESIGN ROLE & PATTERN PARTICIPATION:
 * - Role: Context Data Carrier / Runtime State Container (Composed by WorkflowInstance)
 * - Collaborations:
 *   1. WorkflowInstance (Composition Owner): Each active workflow instance owns
 *      its own independent DataPayload so runtime data in one instance never leaks
 *      into another instance.
 *   2. AssignmentStrategy (Strategy Pattern Context Data): Passed as a const reference
 *      to AssignmentStrategy::assign() so strategies can make data-driven routing
 *      decisions without hardcoding industry-specific attributes into Task classes.
 *   3. WorkflowComponent / Task / EngineMediator: Tasks read and write key-value pairs
 *      (e.g., "backgroundCheck" -> "Passed" in Retail, or "triageLevel" -> "Critical"
 *      in Healthcare) to drive branching and exception paths generically.
 * ============================================================================
 */

#ifndef DATAPAYLOAD_H
#define DATAPAYLOAD_H

#include <string>
#include <map>
#include <iostream>

/**
 * @class DataPayload
 * @brief Stores domain-agnostic runtime key-value data for an active WorkflowInstance.
 */
class DataPayload {
private:
    /** @brief Internal key-value map storing runtime workflow data. */
    std::map<std::string, std::string> values;

public:
    /**
     * @brief Default constructor initializing an empty payload.
     */
    DataPayload();

    /**
     * @brief Copy constructor for cloning payloads across instances if needed.
     * @param other The DataPayload to copy from.
     */
    DataPayload(const DataPayload& other);

    /**
     * @brief Destructor.
     */
    ~DataPayload();

    /**
     * @brief Inserts or updates a key-value pair in the payload.
     * @param key The identifier key (e.g., "employeeRole", "approvalStatus").
     * @param value The string value associated with the key.
     */
    void set(const std::string& key, const std::string& value);

    /**
     * @brief Retrieves the value associated with a given key.
     * @param key The key to look up.
     * @return The value string if found, or an empty string "" if the key does not exist.
     */
    std::string get(const std::string& key) const;

    /**
     * @brief Checks whether a specific key exists in the payload.
     * @param key The key to check.
     * @return true if the key exists in the map, false otherwise.
     */
    bool has(const std::string& key) const;

    /**
     * @brief Removes a key-value pair from the payload if present.
     * @param key The key to remove.
     * @return true if an element was erased, false if the key was not found.
     */
    bool remove(const std::string& key);

    /**
     * @brief Clears all entries from the payload.
     */
    void clear();

    /**
     * @brief Returns a read-only reference to the underlying map for inspection/TUI display.
     * @return Const reference to the internal std::map.
     */
    const std::map<std::string, std::string>& getAll() const;

    /**
     * @brief Utility method to print the current payload contents to standard output (for TUI).
     */
    void printPayload() const;
};

#endif // DATAPAYLOAD_H
