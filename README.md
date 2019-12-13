[![Build Status](https://travis-ci.com/songshuaisong/Socket-TCP-FileTransfer.svg?branch=master)](https://travis-ci.com/songshuaisong/Socket-TCP-FileTransfer)

# Socket-TCP-FileTransfer

The program of uploading and downloading files based on socket TCP under Linux,adopts C-S mode.
First of all, the client can choose whether to upload or download files.
You need to start the server first, and then the client.
   
Important:
    At present, this test code is a schematic code, and there is no error code to improve the relevant situation.

The initial state of the project is shown in the figure:

![image](https://github.com/songshuaisong/Socket-TCP-FileTransfer/blob/master/images/tree-1.png)

If you want to build the project, you only need to use the command "make", shown in the figure:

![image](https://github.com/songshuaisong/Socket-TCP-FileTransfer/blob/master/images/make.png)
    
After make, the client and server executable programs will be generated, is shown in the figure:

![image](https://github.com/songshuaisong/Socket-TCP-FileTransfer/blob/master/images/tree-2.png)

# Server
    In the current directory, use the command:
    
    -- cd bin-server
    -- ./server 127.0.0.1 9999 
    
And then start the server, is shown in the figure:

![image](https://github.com/songshuaisong/Socket-TCP-FileTransfer/blob/master/images/server.png)

# Client
    In the current directory, use the command:
    
    -- cd bin-client
    -- ./client 127.0.0.1 9999 
    
And then start the client, is shown in the figure:

![image](https://github.com/songshuaisong/Socket-TCP-FileTransfer/blob/master/images/client.png)
