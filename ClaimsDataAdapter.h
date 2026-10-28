#ifndef CLAIMSDATAADAPTER_H
#define CLAIMSDATAADAPTER_H

#include <string>
#include "DataAdapter.h"
#include "ClaimsSystem.h"

//adapter for the insurance claims system
class ClaimsDataAdapter: public DataAdapter{
    private:
        ClaimsSystem system; //the adaptee, owned by value

        //"Pending" / "Approved" / "Denied" -> standard status text
        std::string translateStatus(const std::string& status) const;

    public:
        ClaimsDataAdapter();

        bool adapt(const std::string& id, DataPayload& out);
};

#endif
