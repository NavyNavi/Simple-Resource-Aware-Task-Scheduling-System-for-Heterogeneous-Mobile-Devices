#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <jni.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <android/log.h>

class Node {
    public:
        long id;

        int getCriticality() const { return criticality; }
        virtual int execute() = 0;
        virtual int setCriticality() = 0;
        virtual void cleanup() = 0;


    protected:
        int criticality = 0;

};

#endif