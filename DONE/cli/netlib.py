import ctypes
# importo la libreria shared object
C_library = ctypes.CDLL("./obj/netlib.so")
# signatures delle funzioni
initEnv = C_library.initEnv
initEnv.argtypes = []
initEnv.restype = ctypes.c_int
addNode = C_library.addNode
addNode.argtypes = [ctypes.c_char_p, ctypes.c_char]
addNode.restype = ctypes.c_int
delNode = C_library.delNode
delNode.argtypes = [ctypes.c_char_p]
delNode.restype = ctypes.c_int
addExternalInterface = C_library.addExternalInterface
addExternalInterface.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
addExternalInterface.restype = ctypes.c_int
addSwitch = C_library.addSwitch
addSwitch.argtypes = [ctypes.c_char_p]
addSwitch.restype = ctypes.c_int
delSwitch = C_library.delSwitch
delSwitch.argtypes = [ctypes.c_char_p]
delSwitch.restype = ctypes.c_int
addCableBetweenNodes = C_library.addCableBetweenNodes
addCableBetweenNodes.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
addCableBetweenNodes.restype = ctypes.c_int
delCableBetweenNodes = C_library.delCableBetweenNodes
delCableBetweenNodes.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
delCableBetweenNodes.restype = ctypes.c_int
addCableBetweenNodeAndSwitch = C_library.addCableBetweenNodeAndSwitch
addCableBetweenNodeAndSwitch.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
addCableBetweenNodeAndSwitch.restype = ctypes.c_int
delCableBetweenNodeAndSwitch = C_library.delCableBetweenNodeAndSwitch
delCableBetweenNodeAndSwitch.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
delCableBetweenNodeAndSwitch.restype = ctypes.c_int
addCableBetweenSwitches = C_library.addCableBetweenSwitches
addCableBetweenSwitches.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
addCableBetweenSwitches.restype = ctypes.c_int
delCableBetweenSwitches = C_library.delCableBetweenSwitches
delCableBetweenSwitches.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
delCableBetweenSwitches.restype = ctypes.c_int
sendNetworkSetupCommand = C_library.sendNetworkSetupCommand
sendNetworkSetupCommand.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
sendNetworkSetupCommand.restype = ctypes.c_int
openNodeShell = C_library.openNodeShell
openNodeShell.argtypes = [ctypes.c_char_p]
openNodeShell.restype = ctypes.c_int

def help():
    print("""
Hello there, you can call again this command with the `help()` function.

This is an interactive python shell which provides useful bindings for your network simulation functions. Here are some of those:

addNode(name:bytes, type:bytes) -> int
delNode(name:bytes) -> int

addExternalInterface(name:bytes, interface:bytes) -> int

addSwitch(name:bytes) -> int
delSwitch(name:bytes) -> int

addCableBetweenNodes(firstNode:bytes, secondNode:bytes) -> int
delCableBetweenNodes(firstNode:bytes, secondNode:bytes) -> int

addCableBetweenNodeAndSwitch(nodeName:bytes, switchName:bytes) -> int
delCableBetweenNodeAndSwitch(nodeName:bytes, switchName:bytes) -> int

addCableBetweenSwitches(firstSwitch:bytes, secondSwitch:bytes) -> int
delCableBetweenSwitches(firstSwitch:bytes, secondSwitch:bytes) -> int

sendNetworkSetupCommand(name:bytes, command:bytes) -> int
openNodeShell(name:bytes) -> int

""")
initEnv()
help()
