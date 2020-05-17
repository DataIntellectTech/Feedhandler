# Repository Outline

The attachted repository outlines three designs for dynalically loaded kdb+ feedhandlers. Each example builds on the previous design, finishing with a pattern which can be extendted to efficiently ingest data from any reasonably standard source with an available C api. 
The purpose of the code samples is to provide an outline design that a reader may follow, but it should be understood the samples themselves are designed to illustraate genral points, rather than be used directly. A more rigirious implemenation of the third example can be found [here](https://github.com/AquaQAnalytics/TorQ-Solace/).  

## Set up instructions

The examples require access to a revcent version of kdb+ (>3.0). Otherwisethe examples should run on a standard linux machine. 
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

## Set up Instructions

### Install Solace Broker

Information on how to set up and install a solace broker can be found [here](https://docs.solace.com/Solace-SW-Broker-Set-Up/Docker-Containers/Set-Up-Single-Linux-Container.htm)

Once installed, run the below command to find the ip of the broker.
```
@homer:~$ ps -aef | grep docker
root      1068 32392  0 Apr03 ?        00:00:00 /usr/bin/docker-proxy -proto tcp -host-ip 0.0.0.0 -host-port 55559 -container-ip 172.17.0.6 -container-port 55555
```
