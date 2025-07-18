## SSDP-Scanner ##

Scans for SSDP servers using the multicast msearch address of 239.255.255.250.
Original goal of this application was to use SSDP and ECP to control a roku as per:
https://developer.roku.com/en-ca/docs/developer-program/dev-tools/external-control-api.md
Though other products use SSDP as well further investigation on how to use this should eventually be done.

There is also a Samsung Remote ip here done in python:
https://pypi.org/project/samsungctl/

Looking at ports that were open I found on my TU7000 55
davidtu@calysto:~/work/ssdp-scanner$ nmap -Pn -p 1-65535 172.16.1.74
Starting Nmap 7.94SVN ( https://nmap.org ) at 2025-07-09 15:54 CST
Nmap scan report for 172.16.1.74
Host is up (0.024s latency).
...

Port 7678 has a sort of API service on 172.168.1.74:7678/nsservice
This is advertised when queries are made to 239.255.255.250:1900.
The Samsung TV IP will see these queries and send an SSDP protocol response as part of an ssdp:discover message  to be the actual http urn port that is used for nsservice as part of upnp as it is replied to queries sent to 239.255.255.250:1900 to the source port.

### Build Instructions

On Ubuntu I have QT installed in this directory
/opt/Qt
and can just run the build.sh script from the directory that contains the source doing
./build.sh


### Todo: ###
- Investigate using an SSDP C library to simplify this code
- Add unit tests
- Add functionality for search
- Identify what device was found by querying location data.
