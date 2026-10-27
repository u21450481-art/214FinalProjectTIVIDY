#ifndef RETURNSDATAADAPTER_H
#define RETURNSDATAADAPTER_H

#include <string>
#include <vector>
#include "DataAdapter.h"
#include "ReturnsSystem.h"

//adapter for the retail returns system
class ReturnsDataAdapter: public DataAdapter{
    private:
        ReturnsSystem system; //the adaptee, owned by value

        //"0" / "1" / "2" -> standard status text (note: different meaning from the HR codes)
        std::string translateStatus(const std::string& code) const;

        //cuts "a|b|c" into {"a","b","c"}
        std::vector<std::string> split(const std::string& line) const;

    public:
        ReturnsDataAdapter();

        bool adapt(const std::string& id, DataPayload& out);
};

#endif
