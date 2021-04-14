#ifndef ABSTRACT_SAS_H
#define ABSTRACT_SAS_H

#include <string>
#include <vector>

class AbstractSas {
public:
    AbstractSas() = default;
    virtual ~AbstractSas() = default;
    virtual void set_version(int ver) = 0;
    virtual void set_metric(int met) = 0;
    virtual void set_num_mutex(int num) = 0;
    virtual void exec_downward_bin() const = 0;
    virtual int exec() const = 0;
    virtual void write_str(std::string str, int fd) const = 0;
    virtual void set_initial_state(std::vector<int> state) = 0;
    virtual void conc_str(std::string str) = 0;
    virtual void set_output_sas() = 0;
    virtual std::vector<int> get_initial_state() const = 0;
};

#endif