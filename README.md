# Repository Outline

The attachted repository outlines three designs for dynalically loaded kdb+ feedhandlers. Each example builds on the prvious design finishing on a pattern which can be extedted to directly ingest data from any reasonably standard source with an available C api. 
The purpose of the code samples is to provide an outline design, but the sample themselves are designe dto illustare the genral points, rather than be used as code starting points. A more rigirious implemenation ofthe third example can be found [here](https://www.quora.com).  

## Solace

Solace provides a broker that handles messages sent between processes. This application is comparable to a standard kdb+ tickerplant, that publishes messages to real-time subscribers filtered by topic. The code in this repo allows kdb+ to communicate with any solace messaging framework. The API provides a translation from most solace datatypes to kdb+ and vice versa.

## Set up Instructions

### Install Solace Broker

Information on how to set up and install a solace broker can be found [here](https://docs.solace.com/Solace-SW-Broker-Set-Up/Docker-Containers/Set-Up-Single-Linux-Container.htm)

Once installed, run the below command to find the ip of the broker.
```
@homer:~$ ps -aef | grep docker
root      1068 32392  0 Apr03 ?        00:00:00 /usr/bin/docker-proxy -proto tcp -host-ip 0.0.0.0 -host-port 55559 -container-ip 172.17.0.6 -container-port 55555
```
