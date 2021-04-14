#include "sas_proxy.h"
#include "abstract_sas.h"

void SasProxy::exec() const {
    sas->exec();
}