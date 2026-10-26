#ifndef HRDATAADAPTER_H
#define HRDATAADAPTER_H

#include <string>
#include "DataAdapter.h"
#include "LegacyHrSystem.h"

/**
 * Adapter for the legacy HR system (used by employee onboarding).
 * Reads a positional row and turns it into the standard keys.
 */
class HrDataAdapter: public DataAdapter{
    private:
        LegacyHrSystem system; //the adaptee, owned by value

        //HR status code ("1", "3", "9") -> standard status text
        std::string translateStatus(const std::string& code) const;

        //turns "12" into 12. Returns false for anything that is not a plain number
        bool toNumber(const std::string& text, int& number) const;

    public:
        HrDataAdapter();

        /** id is the HR row number as text, e.g. "1". Fills name, status and source. */
        bool adapt(const std::string& id, DataPayload& out);
};

#endif
