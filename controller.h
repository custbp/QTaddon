#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "util.h"

class CmdValidator {
public:
    CmdValidator();
    ~CmdValidator();

    //проверим в целом
    static int check(std::string input);
    //распарсить на команду и параметры
};

class CmdController {
public:
    CmdController();
    ~CmdController();

    int handle_get(std::string in);
};

#endif