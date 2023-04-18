#include <iostream>
#include <cstring>
#include <proc/readproc.h>

using namespace std;

bool isSshConnected()
{
    bool isSsh = false;
    pid_t pid = getpid();

    while (pid != 0)
    {
        PROCTAB* proc_tab = openproc(PROC_FILLSTAT | PROC_FILLSTATUS | PROC_FILLMEM | PROC_PID, &pid);
        proc_t* proc_ptr;

        if (proc_ptr = readproc(proc_tab, NULL))
        {
            std::cout << "Parent PID: " << proc_ptr->tid << ", Name: " << proc_ptr->cmd << std::endl;
            if (!isSsh)
            {
                if (!strcmp(proc_ptr->cmd, "ssh") || !strcmp(proc_ptr->cmd, "sshd"))
                {
                    isSsh = true;
                }
            }
            pid = proc_ptr->ppid;
        } 
        else 
        {
            std::cerr << "Impossible de lire les informations du processus avec le PID : " << pid << std::endl;
            break;
        }

        freeproc(proc_ptr);
        closeproc(proc_tab);
    }

    //std::cout << "PID: 1, Nom: init/systemd" << std::endl;
    return isSsh;
}


int main() 
{
    if (isSshConnected())
    {
        printf("The program is launched via an SSH connection.\n");
    }
    else
    {
        printf("The program is not launched via an SSH connection.\n");
    }

    return 0;
}
