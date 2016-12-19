#include "controller.h"

CmdController::CmdController() {
    DEBUG printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
};

CmdController::~CmdController() {
    DEBUG printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);

};

int CmdController::handle_get(std::string in) {
    DEBUG printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
    
};