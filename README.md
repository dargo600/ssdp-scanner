## SSDP-Scanner ##


Scans for SSDP servers using the multicast msearch address of 239.255.255.250.
Original goal of this application was to use SSDP and ECP to control a roku as per:
https://developer.roku.com/en-ca/docs/developer-program/dev-tools/external-control-api.md
Though other products use SSDP as well further investigation on how to use this should eventually be done.

### Todo: ###
- Investigate using an SSDP C library to simplify this code
- Add unit tests
- Add functionality for search
- Identify what device was found by querying location data.
