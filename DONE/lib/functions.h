//void helloworld(settings_t *settings);
void placelink(settings_t *settings);
//void placehub(settings_t *settings);
void placeswitch(settings_t *settings);
void placerouter(settings_t *settings);
void placehost(settings_t *settings);
void placeexternalinterface(settings_t *settings);
void placeexternalnattedinterface(settings_t *settings);
void placeInternet(settings_t *settings);
void placeText(settings_t *settings);
void deleteNode(settings_t *settings);

void exportDoneScript(settings_t *settings);

void placeRectangle(settings_t *settings);

void quit(settings_t *settings);

void initEnvironment();
void start(settings_t *settings);
void stop(settings_t *settings);
void openShell(settings_t *settings);

void prepareToOpenProject(settings_t *settings);
void openProject(settings_t *settings);

void prepareToSaveProject(settings_t *settings);
void saveProject(settings_t *settings);

void clearCanvas(settings_t *settings);

void populateInterfaceOptionsWrapper(settings_t *settings);

void trackChosenInterfBinding(settings_t *settings);

void getData(settings_t*settings,interface_t*interface);
int validateIP(settings_t*settings,char*tmpIp);

void becomeClient(settings_t*settings);
void becomeServer(settings_t*settings);

void getWriteLock(settings_t *settings);
void releaseWriteLock(settings_t *settings);
void getReadLock(settings_t *settings);
void releaseReadLock(settings_t *settings);

void * actAsServer(void * vargp);
