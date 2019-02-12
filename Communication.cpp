//
// Created by gillis on 20/02/2017.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "Communication.h"
#include "lib/easylogginpp++.h"

bool game::Communication::init() {
    struct sockaddr_un address;

    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("socket() failed\n");
        return false;
    }

    /* start with a clean address structure */
    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, path.c_str());
    //snprintf(address.sun_path, UNIX_PATH_MAX, "./demo_socket");

    if(connect(socket_fd,
               (struct sockaddr *) &address,
               sizeof(struct sockaddr_un)) != 0)
    {
        printf("connect() failed\n");
        return false;
    }
    return true;
}

bool game::Communication::send(const char *data) {
    char buffer[8096];
    int nbytes = snprintf(buffer, 8096, data);
    int n = write(socket_fd, buffer, nbytes);

    //LOG(INFO) << "complete socket write with result: " << n;


    return true;
}
