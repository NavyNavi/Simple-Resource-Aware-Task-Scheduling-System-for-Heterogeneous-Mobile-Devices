#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <jni.h>
#include <string>
#include <stdio.h>
#include <vector>

class Node {
    public:
        long id;
        bool critical = false;

        const bool getCriticality() const { return critical; }
        virtual int execute() = 0;
        virtual int setCriticality() = 0;
        virtual void cleanup() = 0;


    protected:
        int criticality = 0;
};

#endif