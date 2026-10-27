#ifndef STANDARDKEYS_H
#define STANDARDKEYS_H

#include <string>

//the standard keys and status values every adapter writes into the data
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
