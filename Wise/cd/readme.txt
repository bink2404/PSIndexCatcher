The following changes are required to the maker.ini file :-

[APIClients]
;------------------------------ API Clients ------------------------------
emDEX=Standard,emDEX application,C:\PROGRA~1\EMDEX\emDEX.dll, all


The following registry entry is also required, and should be set to the directory that emDEX was installed in :-

HKEY_LOCAL_MACHINE\Software\emDEX\Directory 


These changes should automatically be added by the installation program.