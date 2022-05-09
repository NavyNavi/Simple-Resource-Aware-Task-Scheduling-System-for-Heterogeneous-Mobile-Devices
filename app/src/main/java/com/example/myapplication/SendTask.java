package com.example.myapplication;

import android.app.NotificationManager;
import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

class SendTask extends AsyncTask<Object, Void,Void> {
    Socket socket;
    ServerSocket serverSocket;
    String hostAdd;
    Context context;
    int ownership;
    byte[] serializedTask;

    public SendTask(Context context, byte[] serializedTask){
        Log.d("SendTask", "Initiating sender thread.");
    }

    public static boolean sendFile(Socket socket, byte[] serializedTask) {
        Log.d("SendTask","sending task.");
        if (serializedTask == null){
            Log.d("SendTask","task not set.");
            return false;
        }

        try {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(serializedTask, 0, 64);
            outputStream.close();
        } catch (IOException e) {
            return false;
        }
        return true;
    }

    @Override
    protected Void doInBackground(Object... objects) {
        Log.d("SendTask", "Sending in background.");
        socket= new Socket();
        hostAdd=((InetAddress)objects[0]).getHostAddress();
        context = (Context) objects[1];
        ownership = (int) objects[2];
        switch (ownership){
            case 1:
                try {
                    Log.d("SendTask", "Sending as server");
                    serverSocket=new ServerSocket(8888);
                    socket=serverSocket.accept();
                    sendFile(socket,serializedTask);
                    serverSocket.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case 2:
                try {
                    Log.d("SendTask", "Sending as client");
                    socket.connect(new InetSocketAddress(hostAdd,8888),500);
                    sendFile(socket,serializedTask);
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
        }
        return null;
    }
}
