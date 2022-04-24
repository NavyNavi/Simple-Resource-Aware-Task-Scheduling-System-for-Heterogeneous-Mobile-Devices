#include <jni.h>
#include <string>
#include <stdio.h>
#include <android/log.h>
#include "result-container.cpp"

class ResultTicket {
    public:
        bool ready = false;
        ResultContainer* result = nullptr;

        int getResult() {
            if (ready){
                return result->result;
            }
            return -1;
        }
};