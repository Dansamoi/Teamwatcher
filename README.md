# Teamwatcher

This is a remote control program written in C\C++, with the use of WinAPI. 

All the aspects of the project are self implemented as much as possible. I tried to use only the most basic libraries I can: WinAPI which includes WinSock2, Winuser, GDI and more.

The program has full UI, with easy to use menus to host, connect and control the host machine.
The software streams the host's screen and give the client full control using the mouse and keyboard, the similar way he controls his own computer.

The idea is similar to TeamViewer.

The program uses TCP + UDP protocols for the networking side of the project.

The encryption include shared secret using Diffie-Hellman and one of the most basic and simple symmetric encryptions - XOR encryption.

The program uses Asynchronous messages system, and multi-threading.

More about the project you can read in the attached pdf.

The PDF itself has a lot of asspects of the project, but a lot of unnessessery information which was written just because It was in the format the school wanted: such as about SQL, XML and more, which have no connection to the project and were written just to fit the school format.
