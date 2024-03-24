#include "./headers/composer.h"

int main()
{

    addNode("node1", 'h');
    //addNode("node2", 'r');

    addSwitch("s1");
    addCableBetweenNodeAndSwitch("node1", "s1");

    delNode("node1");
    //delNode("node2");

    delSwitch("s1");

    //delCableBetweenNodeAndSwitch("node1", "s1");

    return 0;
}