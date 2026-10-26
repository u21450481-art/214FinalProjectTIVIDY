#ifndef DATAADAPTER_H
#define DATAADAPTER_H

#include <string>

class DataPayload;

/**
 * Target of the Adapter pattern. The engine only knows this interface, so it
 * can load data from any external system without knowing how that system stores it.
 */
class DataAdapter{
    public:
        virtual ~DataAdapter();

        /**
        * Looks up the record with this id in the external system and writes it into
        * out using the standard keys (see StandardKeys.h).
        * Returns true if the record was found and out was filled.
        * Returns false if the id is invalid or the record does not exist, and in that
        * case out is left exactly as it was.
        */
        virtual bool adapt(const std::string& id, DataPayload& out) = 0;
};

#endif
