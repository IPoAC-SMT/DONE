#include "../lib/settings.h"
// robe type create settings, get/set settings, etc

void toggleInterface(settings_t* settings){
    settings->interfaceHidden=!settings->interfaceHidden;
}
