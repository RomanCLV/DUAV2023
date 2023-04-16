import psutil


def isSshConnected(display_procces_info=True):
    is_ssh = False

    # PID du processus dont on veut afficher les parents
    current_process = psutil.Process()
    pid = current_process.ppid()

    print("current PID:", pid, "Name:", current_process.name())

    # Tant que le PID du processus parent n'est pas égal à 0 (le PID du processus init)
    while pid != 0:
        try:
            parent = psutil.Process(pid)
            pid = parent.ppid()

            if display_procces_info:
                print("Parent PID:", pid, "Name:", parent.name())
            
            if not is_ssh and parent.name() in ["ssh", "sshd"]:
                is_ssh = True
                    
        except psutil.NoSuchProcess:
            break
    return is_ssh


def main():
    if isSshConnected():
        print("The program is launched via an SSH connection.")
    else:
        print("The program is not launched via an SSH connection.")


if __name__ == '__main__':
    main()
