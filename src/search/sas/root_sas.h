#ifndef TASKS_ROOT_SAS_H
#define TASKS_ROOT_SAS_H

#include "../abstract_sas.h"
#include "../abstract_task.h"

namespace sas {
extern std::shared_ptr<AbstractSas> g_root_sas;
extern void read_root_sas(const AbstractTask &task);
}
#endif
