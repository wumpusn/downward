#include "root_sas.h"
#include "../tasks/root_task.h"

#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <complex>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>

using namespace std;

namespace sas {
shared_ptr<AbstractSas> g_root_sas = nullptr;
void read_root_sas() {
    assert(!g_root_sas);
    g_root_sas = make_shared<RootSas>();
}
}


void RootSas::set_version(int ver) {
    version = ver;
}

void RootSas::set_metric(int met) {
    metric = met;
}

void RootSas::set_num_mutex(int num) {
    num_mutex = num;
}

void RootSas::set_initial_state(vector<int> state) {
    initial_state.resize(state.size());
    for(int i = 0; i < state.size(); i++) {
        initial_state[i] = state[i];
    }
}

void RootSas::exec_downward_bin() const {
    char *args[5];
    extern char **environ;

    args[0] = "downward";
    args[1] = "--search";
    args[2] = "astar(blind())";
    args[3] = "--internal-plan-file";
    args[4] = "sas_plan";
    args[5] = "--is-child-process";
    args[6] = NULL;


    const char *fd_path = "/mnt/c/Users/yutat/workspace/lab/downward-projects/downward/builds/release/bin/downward";
    if ( execve(fd_path, args, environ) < 0) {
        perror("execve");
        exit(1);
    }
    // if( execlp("/bin/cat", "cat", NULL) < 0) {
    //     perror("execlp");
    //     exit(1);
    // }
}

void RootSas::conc_str(string str) {
    str += "\n";
    output_sas += str;
}

void RootSas::set_output_sas() {
    output_sas = "";
    assert(output_sas == "");
    conc_str("begin_version");
    conc_str(to_string(version));
    conc_str("end_version");
    conc_str("begin_metric");
    conc_str(to_string(metric));
    conc_str("end_metric");
    conc_str(to_string(tasks::g_root_task->get_num_variables()));
    for(int i = 0; i < tasks::g_root_task->get_num_variables(); i++) {
        conc_str("begin_variable");
        conc_str(tasks::g_root_task->get_variable_name(i));
        conc_str(to_string(tasks::g_root_task->get_variable_axiom_layer(i)));
        conc_str(to_string(tasks::g_root_task->get_variable_domain_size(i)));
        for(int j = 0; j < tasks::g_root_task->get_variable_domain_size(i); j++) {
            FactPair fact(i, j);
            conc_str(tasks::g_root_task->get_fact_name(fact));
        }
        conc_str("end_variable");
    }
    // mutex
    conc_str(to_string(num_mutex));
    for(auto mutex : mutexs) {
        conc_str("begin_mutex_group");
        conc_str(to_string(mutex.size()));
        for (auto m : mutex) {
            conc_str(
                to_string(m.first) + " " + to_string(m.second)
            );
        }
        conc_str("end_mutex_group");
    }
    conc_str("begin_state");
    for(int i = 0; i < initial_state.size(); i++) {
        conc_str(to_string(initial_state[i]));
    }
    conc_str("end_state");
    conc_str("begin_goal");
    conc_str(to_string(tasks::g_root_task->get_num_goals()));
    for(int i = 0; i < tasks::g_root_task->get_num_goals(); i++) {
        FactPair goal = tasks::g_root_task->get_goal_fact(i);
        string out = to_string(goal.var) + " " + to_string(goal.value);
        conc_str(out);
    }
    conc_str("end_goal");
    conc_str(to_string(tasks::g_root_task->get_num_operators()));
    for(int i = 0; i < tasks::g_root_task->get_num_operators(); i++) {
        conc_str("begin_operator");
        conc_str(tasks::g_root_task->get_operator_name(i, false));
        int num_preconditions = tasks::g_root_task->get_num_operator_preconditions(i, false);
        int num_effects = tasks::g_root_task->get_num_operator_effects(i, false);
        int overlapped_num = 0;
        vector<vector<int>> output_effects;
        for(int j = 0; j < num_effects; j++) {
            vector<int> v;
            FactPair eff = tasks::g_root_task->get_operator_effect(i, j, false);
            int num_effect_condition = tasks::g_root_task->get_num_operator_effect_conditions(i, j, false);
            v.push_back(num_effect_condition);
            v.push_back(eff.var);
            v.push_back(-1);
            v.push_back(eff.value);
            for(int k = 0; k < num_preconditions; k++) {
                FactPair pred = tasks::g_root_task->get_operator_precondition(i, k, false);
                if(eff.var == pred.var) {
                    v.at(2) = pred.value;
                    overlapped_num++;
                    break;
                }
            }
            output_effects.emplace_back(v);
        }
        int num_pred = num_preconditions - overlapped_num;
        conc_str(to_string(num_pred));
        for(int j = 0; j < num_pred; j++) {
            string pred_str;
            FactPair pred = tasks::g_root_task->get_operator_precondition(i, j, false);
            pred_str = to_string(pred.var) + " " + to_string(pred.value);
            conc_str(pred_str);
        }
        conc_str(to_string(num_effects));
        for(int j = 0; j < num_effects; j++) {
            string out_str;
            out_str = to_string(output_effects.at(j).at(0))
                + " "
                + to_string(output_effects.at(j).at(1))
                + " "
                + to_string(output_effects.at(j).at(2))
                + " "
                + to_string(output_effects.at(j).at(3));
            conc_str(out_str);
        }
        conc_str(to_string(tasks::g_root_task->get_operator_cost(i, false)));
        conc_str("end_operator");
    }
    conc_str(to_string(tasks::g_root_task->get_num_axioms()));
}

string get_date_str() {
    time_t rawtime;
    const tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y%m%dT%H%M%S", timeinfo);
    string str(buffer);
    return str;
}

int RootSas::exec() const {
    string filename = "tmp/output_sas_repro.txt";
    ofstream writing_file;
    ifstream ifs(filename);
    if (ifs.is_open()) {
        writing_file.open(filename, ios::trunc);
    }
    else {
        writing_file.open(filename, ios::out);
    }
    writing_file << output_sas;

    int fd, child, status;

    fd = open(filename.c_str(), O_RDONLY);
    if ( fd < 0) {
        perror("open");
        exit(1);
    }
    if ((child = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    if (child == 0) {
        dup2(fd, STDIN_FILENO);
        close(fd);
        exec_downward_bin();
    }
    else {
        if(wait(&status) < 0) {
            perror("wait");
            exit(1);
        }
        printf("The child (pid=%d) exited with status(%d).\n",
            child, WEXITSTATUS(status));
    }
}

void RootSas::write_str(string str, file_discriptor fd) const {
    const char *send_str = str.c_str();
    write(fd, send_str, strlen(send_str) + 1);
}

vector<int> RootSas::get_initial_state() const {
    return initial_state;
}

void RootSas::set_mutexs(std::vector<std::vector<std::pair<int, int>>> mutex_elems) {
    mutexs.reserve(num_mutex);
    mutexs = mutex_elems;
}