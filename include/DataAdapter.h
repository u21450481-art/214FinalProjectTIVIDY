#ifndef DATAADAPTER_H
#define DATAADAPTER_H

#include <string>

class DataPayload;

//the interface the engine uses to load data from any external system into the standard format
class DataAdapter{
    public:
        virtual ~DataAdapter();

        virtual bool adapt(const std::string& id, DataPayload& out) = 0;
};

#endif
