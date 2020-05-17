# Repository Outline

The attachted repository outlines three designs for dynalically loaded kdb+ feedhandlers. Each example builds on the previous design, finishing with a pattern which can be extendted to efficiently ingest data from any reasonably standard source with an available C api. 
The purpose of the code samples is to provide an outline design that a reader may follow, but it should be understood the samples themselves are designed to illustraate genral points, rather than be used directly. A more rigirious implemenation of the third example can be found [here](https://github.com/AquaQAnalytics/TorQ-Solace/).  

## Set up instructions

The examples require access to a recent version of kdb+ (>3.0). Otherwisethe examples should run on a standard linux machine. 
```
kdb@homer:~$ git clone https://github.com/AquaQAnalytics/Feedhandler.git
Cloning into 'Feedhandler'...
remote: Enumerating objects: 26, done.
remote: Counting objects: 100% (26/26), done.
remote: Compressing objects: 100% (20/20), done.
remote: Total 26 (delta 5), reused 19 (delta 4), pack-reused 0
Unpacking objects: 100% (26/26), done.
kdb@homer:~$ cd Feedhandler/
kdbe@homer:~Feedhandler$ make
rm -f lib/*
gcc c/blog.c c/eg1.c -Iinc/ -DKXVER=3 -fPIC -shared -lpthread -Wno-incompatible-pointer-types -o lib/eg1.so
gcc c/blog.c c/eg2.c -Iinc/ -DKXVER=3 -fPIC -shared -lpthread -Wno-incompatible-pointer-types -o lib/eg2.so
gcc c/blog.c -Iinc/ -DKXVER=3 -fPIC -shared -lpthread -Wno-incompatible-pointer-types c/eg3.c c/cb.c -o lib/eg3.so
solace@homer:~Feedhandler$
solace@homer:~Feedhandler$ q q/blog.q
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems
"Must specify example"
"usage: q q/blog.q -eg eg3 [-freq 100] [-callback slow]"
solace@homer:~Feedhandler$
```
## Outline

Each example design source can be loaded using the main driver file `blog.q`. Each example contains a few common functions which are outlined below.

### `startserver[port;frequency]`
This function starts a thread which mimics a feed or exchange for use in all the example demonstrations. The feed starts listening on a port defined by the first argument and sends a simple "hello world" message at a frequency defined by the second argument. For simplicity the message payload is a constant size. This payload is pushed to a client that connects. By varying the feqency of the updates the behaviour of the different examples can be shown. The connection is via a simple TCP socket.   

### `startclient[port]`
This function starts a feed which connects to the feed (which must be started previously). The feed then recieves messages and passes these to the host kdb+ session. The manner and efficiency in which this data is passed back is different in each of the three examples. 

### `getstats[]`
This function returns statistics relating to how many mesages have been sent by the exchange, how many have been processed by the feed application and potentially how many have been passed back to the main kdb+ application. This functions and its return shows how the different designs relevant message queues behave. 


## Design A - Simple Callback
This is the most basic design and if the feed is either low frequency, or processing time of individual messages is short may be suitable. The example also illustrates the usage of the `sd1` function within the core kdb+ api.
```
#################################
# Example 1(a): Simple consumer with slow message rate
homer$ q q/blog.q -eg eg1
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems

 Info: start_listen threadstarted.
 Info: start_listen Waiting for incoming connections...
 Info: startclient starting to connect to server
q) Info: start_listen connection received
q)("Update ";20:52:40.535;"#### Hello world      0  ####";0;(`sent;`processed)!1 0)
("Update ";20:52:41.535;"#### Hello world      1  ####";1;(`sent;`processed)!2 1)
("Update ";20:52:42.535;"#### Hello world      2  ####";2;(`sent;`processed)!3 2)
("Update ";20:52:43.536;"#### Hello world      3  ####";3;(`sent;`processed)!4 3)
("Update ";20:52:44.536;"#### Hello world      4  ####";4;(`sent;`processed)!5 4)
("Update ";20:52:45.536;"#### Hello world      5  ####";5;(`sent;`processed)!6 5)
("Update ";20:52:46.536;"#### Hello world      6  ####";6;(`sent;`processed)!7 6)
("Update ";20:52:47.536;"#### Hello world      7  ####";7;(`sent;`processed)!8 7)
("Update ";20:52:48.536;"#### Hello world      8  ####";8;(`sent;`processed)!9 8)
("Update ";20:52:49.536;"#### Hello world      9  ####";9;(`sent;`processed)!10 9)
("Update ";20:52:50.536;"#### Hello world     10  ####";10;(`sent;`processed)!11 10)
("Update ";20:52:51.537;"#### Hello world     11  ####";11;(`sent;`processed)!12 11)
("Update ";20:52:52.537;"#### Hello world     12  ####";12;(`sent;`processed)!13 12)
("Update ";20:52:53.537;"#### Hello world     13  ####";13;(`sent;`processed)!14 13)
("Update ";20:52:54.537;"#### Hello world     14  ####";14;(`sent;`processed)!15 14)
\\
#################################
# Example 1(b): Simple consumer with fast message rate and client beginning to fall behind processing messages
homer$ q q/blog.q -eg eg1 -freq 100 -callback slow
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems

 Info: start_listen threadstarted.
 Info: start_listen Waiting for incoming connections...
 Info: startclient starting to connect to server
 Info: start_listen connection received
q)("Update ";20:53:10.077;"#### Hello world      0  ####";0;(`sent;`processed)!1 0)
("Update ";20:53:10.249;"#### Hello world      1  ####";1;(`sent;`processed)!2 1)
("Update ";20:53:10.413;"#### Hello world      2  ####";2;(`sent;`processed)!4 2)
("Update ";20:53:10.573;"#### Hello world      3  ####";3;(`sent;`processed)!5 3)
("Update ";20:53:10.735;"#### Hello world      4  ####";4;(`sent;`processed)!7 4)
("Update ";20:53:10.907;"#### Hello world      5  ####";5;(`sent;`processed)!9 5)
("Update ";20:53:11.067;"#### Hello world      6  ####";6;(`sent;`processed)!10 6)
("Update ";20:53:11.226;"#### Hello world      7  ####";7;(`sent;`processed)!12 7)
("Update ";20:53:11.385;"#### Hello world      8  ####";8;(`sent;`processed)!14 8)
("Update ";20:53:11.545;"#### Hello world      9  ####";9;(`sent;`processed)!15 9)
("Update ";20:53:11.717;"#### Hello world     10  ####";10;(`sent;`processed)!17 10)
("Update ";20:53:11.878;"#### Hello world     11  ####";11;(`sent;`processed)!18 11)
("Update ";20:53:12.040;"#### Hello world     12  ####";12;(`sent;`processed)!20 12)
("Update ";20:53:12.200;"#### Hello world     13  ####";13;(`sent;`processed)!22 13)
("Update ";20:53:12.361;"#### Hello world     14  ####";14;(`sent;`processed)!23 14)
("Update ";20:53:12.523;"#### Hello world     15  ####";15;(`sent;`processed)!25 15)
("Update ";20:53:12.696;"#### Hello world     16  ####";16;(`sent;`processed)!27 16)
("Update ";20:53:12.857;"#### Hello world     17  ####";17;(`sent;`processed)!28 17)
("Update ";20:53:13.017;"#### Hello world     18  ####";18;(`sent;`processed)!30 18)
("Update ";20:53:13.177;"#### Hello world     19  ####";19;(`sent;`processed)!31 19)
("Update ";20:53:13.337;"#### Hello world     20  ####";20;(`sent;`processed)!33 20)
("Update ";20:53:13.497;"#### Hello world     21  ####";21;(`sent;`processed)!35 21)
("Update ";20:53:13.657;"#### Hello world     22  ####";22;(`sent;`processed)!36 22)
("Update ";20:53:13.817;"#### Hello world     23  ####";23;(`sent;`processed)!38 23)
("Update ";20:53:13.977;"#### Hello world     24  ####";24;(`sent;`processed)!39 24)
("Update ";20:53:14.136;"#### Hello world     25  ####";25;(`sent;`processed)!41 25)
\\

########################################
```
In example as the message rate increases or the processing time of the payload of a callback increases the number of items the feed has sent but remain unprocessed by the client increases. Eventually, the exchange socket connection will fill up and in a real life scenario the feed conenction would be terminated.

## Design B - Dual Thread consumer

In this design the client creates a thread specifically for listening to the exchange. Once a message has been removed from the exchange connection it is passed by this thread to the kdb+ main thread via a socket pair. This socket pair acts like a waiting room for messages that are waiting to be processed, but crucially the socket pair is owned by the client application, and messages are retrieved from the exchange socket as quickly as possible. 

```
############################
# Example 2(a): Dual thread consumer
homer$ q q/blog.q -eg eg2
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems

 Info: start_listen threadstarted.
 Info: start_listen Waiting for incoming connections...
 Info: startclient starting to connect to server
 Info: start_listen connection received
q)("Update ";21:07:17.812;0;"#### Hello world      0  ####";(`sent;`received;`processed)!1 1 0)
("Update ";21:07:17.912;1;"#### Hello world      1  ####";(`sent;`received;`processed)!2 2 1)
("Update ";21:07:18.012;2;"#### Hello world      2  ####";(`sent;`received;`processed)!3 3 2)
("Update ";21:07:18.112;3;"#### Hello world      3  ####";(`sent;`received;`processed)!4 4 3)
("Update ";21:07:18.213;4;"#### Hello world      4  ####";(`sent;`received;`processed)!5 5 4)
("Update ";21:07:18.313;5;"#### Hello world      5  ####";(`sent;`received;`processed)!6 6 5)
("Update ";21:07:18.413;6;"#### Hello world      6  ####";(`sent;`received;`processed)!7 7 6)
("Update ";21:07:18.513;7;"#### Hello world      7  ####";(`sent;`received;`processed)!8 8 7)
("Update ";21:07:18.613;8;"#### Hello world      8  ####";(`sent;`received;`processed)!9 9 8)
("Update ";21:07:18.713;9;"#### Hello world      9  ####";(`sent;`received;`processed)!10 10 9)
("Update ";21:07:18.813;10;"#### Hello world     10  ####";(`sent;`received;`processed)!11 11 10)
("Update ";21:07:18.913;11;"#### Hello world     11  ####";(`sent;`received;`processed)!12 12 11)
("Update ";21:07:19.013;12;"#### Hello world     12  ####";(`sent;`received;`processed)!13 13 12)
("Update ";21:07:19.114;13;"#### Hello world     13  ####";(`sent;`received;`processed)!14 14 13)
("Update ";21:07:19.214;14;"#### Hello world     14  ####";(`sent;`received;`processed)!15 15 14)
("Update ";21:07:19.314;15;"#### Hello world     15  ####";(`sent;`received;`processed)!16 16 15)
("Update ";21:07:19.414;16;"#### Hello world     16  ####";(`sent;`received;`processed)!17 17 16)
\\
##############################
# Example 2(b): Dual thread consumer with slow message rate
homer$ q q/blog.q -eg eg2 -freq 100 -callbaqck slow
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems
l64/ 24()core 128387MB solace homer 127.0.1.1 EXPIRE 2020.06.30 AquaQ #55345

 Info: start_listen threadstarted.
 Info: start_listen Waiting for incoming connections...
 Info: startclient starting to connect to server
 Info: start_listen connection received
q)("Update ";21:03:04.240;0;"#### Hello world      0  ####";(`sent;`received;`processed)!1 1 0)
("Update ";21:03:04.412;1;"#### Hello world      1  ####";(`sent;`received;`processed)!2 2 1)
("Update ";21:03:04.573;2;"#### Hello world      2  ####";(`sent;`received;`processed)!4 4 2)
("Update ";21:03:04.733;3;"#### Hello world      3  ####";(`sent;`received;`processed)!5 5 3)
("Update ";21:03:04.893;4;"#### Hello world      4  ####";(`sent;`received;`processed)!7 7 4)
("Update ";21:03:05.052;5;"#### Hello world      5  ####";(`sent;`received;`processed)!9 9 5)
("Update ";21:03:05.211;6;"#### Hello world      6  ####";(`sent;`received;`processed)!10 10 6)
("Update ";21:03:05.370;7;"#### Hello world      7  ####";(`sent;`received;`processed)!12 12 7)
("Update ";21:03:05.528;8;"#### Hello world      8  ####";(`sent;`received;`processed)!13 13 8)
("Update ";21:03:05.687;9;"#### Hello world      9  ####";(`sent;`received;`processed)!15 15 9)
("Update ";21:03:05.846;10;"#### Hello world     10  ####";(`sent;`received;`processed)!17 17 10)
("Update ";21:03:06.005;11;"#### Hello world     11  ####";(`sent;`received;`processed)!18 18 11)
("Update ";21:03:06.164;12;"#### Hello world     12  ####";(`sent;`received;`processed)!20 20 12)
("Update ";21:03:06.323;13;"#### Hello world     13  ####";(`sent;`received;`processed)!21 21 13)
("Update ";21:03:06.482;14;"#### Hello world     14  ####";(`sent;`received;`processed)!23 23 14)
("Update ";21:03:06.641;15;"#### Hello world     15  ####";(`sent;`received;`processed)!24 24 15)
("Update ";21:03:06.800;16;"#### Hello world     16  ####";(`sent;`received;`processed)!26 26 16)
\\
##############################
```
Example 2(a) shows the messages sent by the exchange (`sent`), recieved by the client but not yet passed to the parent kdb+ application (`received`), and those that have been processed by the main kdb+ application (`processed`). For low frequency messages this design performs no better than the first example. In example 2(b), where the processing time of a message has been increased we can see that the messages reciveed by the application stays in line with those sent by the exchange, even thoough the number processed trails significantly. Eventually the sockat pair connection would fill up and the exchange connection would become saturated, however for short periods the socket pair acts like a safety valave, in much the same manner as a chained tickerplant would in a traditional tickerplant setup.

## Design B - Dual Thread consumer with circular buffer

Inthe final example, the previous setup has been improved in two areas. Transferring the data between threads via a network socket has significant overhead. Even if Unix Domain Sockets are used this transfer is inefficent and has been replaced with a memory copy onto a shared space of memeroy created on the head of the client application. Furtehrmore, rather than passing each message individually onto the main client callback function, multiple messages, when available, are drained from buffer and passed back. This gives the client the option to ignore messages and essentially the option to "look a head" when the application has pending messages. In effect this means that the number of events on the client side, may be significantly less than those on the exchange side.

```
###################################
#Example 3:Dual thread consumer with circular buffer and message conflation/aggregation
homer$ q q/blog.q -eg eg3 -freq 100 -callback slow
KDB+ 3.5 2017.11.30 Copyright (C) 1993-2017 Kx Systems


 Info: start_listen threadstarted.
 Info: start_listen Waiting for incoming connections...
 Info: startclient starting to connect to server
 Info: start_listen connection received
q)("Update ";21:09:50.228;1;,`#### Hello world      0  ####;(`sent;`received;`processed;`callbacks)!1 1 1 1)
("Update ";21:09:50.400;2;,`#### Hello world      1  ####;(`sent;`received;`processed;`callbacks)!2 2 2 2)
("Update ";21:09:50.560;4;(`#### Hello world      2  ####;`#### Hello world      3  ####);(`sent;`received;`processed;`callbacks)!4 4 4 3)
("Update ";21:09:50.721;5;,`#### Hello world      4  ####;(`sent;`received;`processed;`callbacks)!5 5 5 4)
("Update ";21:09:50.881;7;(`#### Hello world      5  ####;`#### Hello world      6  ####);(`sent;`received;`processed;`callbacks)!7 7 7 5)
("Update ";21:09:51.041;9;(`#### Hello world      7  ####;`#### Hello world      8  ####);(`sent;`received;`processed;`callbacks)!9 9 9 6)
("Update ";21:09:51.201;10;,`#### Hello world      9  ####;(`sent;`received;`processed;`callbacks)!10 10 10 7)
("Update ";21:09:51.361;12;(`#### Hello world     10  ####;`#### Hello world     11  ####);(`sent;`received;`processed;`callbacks)!12 12 12 8)
("Update ";21:09:51.520;13;,`#### Hello world     12  ####;(`sent;`received;`processed;`callbacks)!13 13 13 9)
("Update ";21:09:51.680;15;(`#### Hello world     13  ####;`#### Hello world     14  ####);(`sent;`received;`processed;`callbacks)!15 15 15 10)
("Update ";21:09:51.851;17;(`#### Hello world     15  ####;`#### Hello world     16  ####);(`sent;`received;`processed;`callbacks)!17 17 17 11)
("Update ";21:09:52.010;18;,`#### Hello world     17  ####;(`sent;`received;`processed;`callbacks)!18 18 18 12)
("Update ";21:09:52.170;20;(`#### Hello world     18  ####;`#### Hello world     19  ####);(`sent;`received;`processed;`callbacks)!20 20 20 13)
("Update ";21:09:52.329;21;,`#### Hello world     20  ####;(`sent;`received;`processed;`callbacks)!21 21 21 14)
("Update ";21:09:52.488;23;(`#### Hello world     21  ####;`#### Hello world     22  ####);(`sent;`received;`processed;`callbacks)!23 23 23 15)
("Update ";21:09:52.648;25;(`#### Hello world     23  ####;`#### Hello world     24  ####);(`sent;`received;`processed;`callbacks)!25 25 25 16)
("Update ";21:09:52.807;26;,`#### Hello world     25  ####;(`sent;`received;`processed;`callbacks)!26 26 26 17)
("Update ";21:09:52.967;28;(`#### Hello world     26  ####;`#### Hello world     27  ####);(`sent;`received;`processed;`callbacks)!28 28 28 18)
("Update ";21:09:53.126;29;,`#### Hello world     28  ####;(`sent;`received;`processed;`callbacks)!29 29 29 19)
("Update ";21:09:53.285;31;(`#### Hello world     29  ####;`#### Hello world     30  ####);(`sent;`received;`processed;`callbacks)!31 31 31 20)
("Update ";21:09:53.444;33;(`#### Hello world     31  ####;`#### Hello world     32  ####);(`sent;`received;`processed;`callbacks)!33 33 33 21)
("Update ";21:09:53.604;34;,`#### Hello world     33  ####;(`sent;`received;`processed;`callbacks)!34 34 34 22)
("Update ";21:09:53.764;36;(`#### Hello world     34  ####;`#### Hello world     35  ####);(`sent;`received;`processed;`callbacks)!36 36 36 23)
###################################
```

### Summary 







