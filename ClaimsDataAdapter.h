#ifndef CLAIMSDATAADAPTER_H
#define CLAIMSDATAADAPTER_H

#include <string>
#include "DataAdapter.h"
#include "ClaimsSystem.h"

/**
 * Adapter for the insurance claims system (used by insurance claim workflows).
 * Renames fullName to name, converts the text status, and passes on the amount.
 */
class ClaimsDataAdapter: public DataAdapter{
    private:
        ClaimsSystem system; //the adaptee, owned by value

        //"Pending" / "Approved" / "Denied" -> standard status text
        std::string translateStatus(const std::string& status) const;

    public:
        ClaimsDataAdapter();

        /** id is the claim reference, e.g. "C-102". Fills name, status, amount and source. */
        bool adapt(const std::string& id, DataPayload& out);
};

#endif
