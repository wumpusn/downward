#include "root_sas.h"


using namespace std;

namespace sas {
shared_ptr<AbstractSas> g_root_sas = nullptr;

class RootSas : public AbstractSas {
    const AbstractTask *task;
public:
    explicit RootSas(const AbstractTask &task)
        : task(&task){}
    ~RootSas() = default;

};

void read_root_sas(const AbstractTask &task) {
    assert(!g_root_sas);
    g_root_sas = make_shared<RootSas>(task);
}
}