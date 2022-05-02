package com.example.myapplication;

import android.app.NotificationManager;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

class ReceiveTask extends AsyncTask<Object, Void,Void> {
    Socket socket;
    ServerSocket serverSocket;
    String hostAdd;
    Context context;
    int ownership;

    public ReceiveTask(Context context){}

    public static boolean receiveFile(Socket socket,String fileName){
        Log.d("WifiDirect", "receive file.");
        String[] name_arr=fileName.split("\\.",2);
        try {
            //save file with timestamp
//            final File f = new File(Environment.getExternalStorageDirectory() + "/D2D"
//                    + "/" + System.currentTimeMillis()
//                    +"."+ name_arr[name_arr.length-1]);
            //save file with original name
            final File f = new File(Environment.getExternalStorageDirectory() + "/D2D"
                    + "/" + fileName);

            File dirs = new File(f.getParent());
            if (!dirs.exists())
                dirs.mkdirs();
            dirs.mkdirs();
            f.createNewFile();
            InputStream inputstream = socket.getInputStream();
            copyFile(inputstream, new FileOutputStream(f));
        }catch (IOException e){
            return false;
        }
        return true;
    }

    public static boolean copyFile(InputStream inputStream, OutputStream out) {
        byte[] buf = new byte[1024];
        int len;
        try {
            while ((len = inputStream.read(buf)) != -1) {
                out.write(buf, 0, len);

            }
            out.close();
            inputStream.close();
        } catch (IOException e) {
//            Log.d("inside copyFile", e.toString());
            return false;
        }
        return true;
    }

    @Override
    protected Void doInBackground(Object... objects) {
        socket= new Socket();
        hostAdd=((InetAddress)objects[0]).getHostAddress();
        context = (Context) objects[1];
        ownership = (int) objects[2];
        switch (ownership){
            case 1:
                try {
                    serverSocket=new ServerSocket(8888);
                    socket=serverSocket.accept();
                    receiveFile(socket,"");
                    serverSocket.close();
                    //CloudFileScreen.hasRequested=false;
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
            case 2:
                try {
                    socket.connect(new InetSocketAddress(hostAdd,8888),500);
                    receiveFile(socket,"");
                    //CloudFileScreen.hasRequested=false;
                } catch (IOException e) {
                    e.printStackTrace();
                }
                break;
        }

        return null;
    }
}