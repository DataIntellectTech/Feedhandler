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

## Design A - Simple Callback

``
#################################
# Example 1: Simple consumer with slow message rate
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
# Example 1: Simple consumer with fast message rate and client beginning to fall behind processing messages
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
``


### Install Solace Broker

