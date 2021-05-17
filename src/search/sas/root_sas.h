#ifndef TASKS_ROOT_SAS_H
#define TASKS_ROOT_SAS_H

#include "../abstract_sas.h"
#include "../abstract_task.h"

#include <vector>
#include <string>

using file_discriptor = int;

namespace sas {
extern std::shared_ptr<AbstractSas> g_root_sas;
extern void read_root_sas();
extern int SAS_CNT;
}


class RootSas : public AbstractSas {
    int version;
    int metric;
    int num_mutex;
    std::string output_sas;
    std::vector<int> initial_state;
    std::vector<std::vector<std::pair<int, int>>> mutexs;
public:
    explicit RootSas(){}
    void set_version(int ver);
    void set_metric(int met);
    void set_num_mutex(int num);
    void read_sas_info();
    void exec_downward_bin() const;
    int exec() const;
    void write_str(std::string str, file_discriptor fd) const;
    void set_initial_state(std::vector<int> state);
    void conc_str(std::string str);
    void set_output_sas();
    std::vector<int> get_initial_state() const;
    void set_mutexs(std::vector<std::vector<std::pair<int, int>>> mutexs);
    std::string zero_pud(int num) const;
};
#endif
