package com.example.myapplication;

public class Task {
    private long id;
    private int funcId;
    private int args;

    Task(long id, int funcId, int args) {
        this.id = id;
        this.funcId = funcId;
        this.args = args;
    }
}
