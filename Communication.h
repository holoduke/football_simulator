//
// Created by gillis on 20/02/2017.
//

#ifndef FOOTBALLSIMULATOR_COMMUNICATION_H
#define FOOTBALLSIMULATOR_COMMUNICATION_H

namespace game {
    class Communication {

    public:
        bool init();
        bool send(const char * data);
        int socket_fd;
        std::string path;
    };


}


#endif //FOOTBALLSIMULATOR_COMMUNICATION_H
