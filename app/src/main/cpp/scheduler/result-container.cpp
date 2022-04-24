#ifndef RESULTCONTAINER_INCLUDED
#define RESULTCONTAINER_INCLUDED

#include <jni.h>
#include <string>
#include <stdio.h>
#include <android/log.h>

class ResultContainer {
    public:
        ResultContainer(bool isException, int result, long duration, long energyConsumption, long id){
            this->isException = isException;
            this->result = result;
            this->duration = duration;
            this->energyConsumption = energyConsumption;
            this->id = id;
        }
        ResultContainer(int result, long id){
            this->result = result;
            this->id = id;
        }
        bool isException = false;
        int result;
        long duration = 0;
        long energyConsumption = 0;
        long id;
};

#endif