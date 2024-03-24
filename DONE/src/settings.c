#include "../lib/settings.h"
// robe tipo create settings, get/set settings, etc

void toggleInterface(settings_t* settings){
    settings->interfaceHidden=!settings->interfaceHidden;
}
