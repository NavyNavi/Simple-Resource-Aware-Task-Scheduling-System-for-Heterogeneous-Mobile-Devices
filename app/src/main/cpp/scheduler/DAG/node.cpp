#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <jni.h>
#include <string>
#include <stdio.h>
#include <vector>

class Node {
    public:
        long id;

        int getCriticality() { return criticality; }
        virtual int execute() = 0;
        virtual int setCriticality() = 0;
        virtual void cleanup() = 0;


    protected:
        int criticality = 0;

};

#endif