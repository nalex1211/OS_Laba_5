#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstdlib>

int f(int x) {
    return x + 1;
}

int g(int x) {
    return x * 2;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <number>" << std::endl;
        return 1;
    }

    int x = std::stoi(argv[1]);

    int sockets[2];
    pid_t pid;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) < 0) {
        perror("socketpair");
        exit(1);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        close(sockets[0]);
        int result = f(x);
        write(sockets[1], &result, sizeof(result));
        close(sockets[1]);
    } else {
        close(sockets[1]);
        int result = g(x);
        int child_result;
        read(sockets[0], &child_result, sizeof(child_result));
        close(sockets[0]);

        int final_result = (result == 0 || child_result == 0) ? 0 : result * child_result;
        std::cout << "Result of f(x) * g(x): " << final_result << std::endl;
    }

    return 0;
}