#ifndef STANDARDKEYS_H
#define STANDARDKEYS_H

#include <string>

/**
 * The standard vocabulary every adapter writes into a DataPayload.
 * Workflows and the engine read ONLY these keys and values, never the
 * field names or codes of an external system.
 */
namespace StandardKeys{
    //keys
    const std::string NAME = "name"; //the person the record is about
    const std::string STATUS = "status"; //one of the STATUS_ values below
    const std::string AMOUNT = "amount"; //money amount as text, only if the source has one
    const std::string SOURCE = "source"; //which external system the data came from

    //standard status values
    const std::string STATUS_PENDING = "pending";
    const std::string STATUS_APPROVED = "approved";
    const std::string STATUS_REJECTED = "rejected";
    const std::string STATUS_UNKNOWN = "unknown"; //the source used a code we do not recognise
}

#endif
