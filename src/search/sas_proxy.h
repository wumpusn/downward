#ifndef SAS_PROXY_H
#define SAS_PROXY_H

#include "abstract_sas.h"

class SasProxy {
    const AbstractSas *sas;
public:
    explicit SasProxy(const AbstractSas &sas)
        : sas(&sas) {}
    ~SasProxy() = default;
    void exec() const;
};

#endif