# XCM BaseCOM STM32 sample

This project is a simple test code used to show the usage of the [xcm code generation feature](https://github.com/Freshdouble/xcode) over the [smp link layer protocol](https://github.com/Freshdouble/libsmp).

The main portion of the code is inside the commhandler.cpp file which is mainly glue code between C and C++.

The file xcode.hpp is autogenerated by xcode.

## Building the packet definition file

To build the packet definition file (xcode.hpp) the xcode generator is used:

`xcodegen -f transmission.xml -o /Core/Inc`

## PC connection

To receive and send data using the xcmparser program start it with the command:

`xcmparser -f .\transmission.xml -p "serial:COM4:115200|smp" -v --enableinterface --noforward`

The noforward option is used to supress the output as a udpstream, but you can omit this option and watch the incommind data on the localhost at udp port 9000.

To send commands open the interactive terminal mode by pressing the t key. Type 0 to select the SetLEDStatus message and enter the message overview. Then type 0 to change the ledstatus entry inside the Argument symbol. Type "true" and hit enter to change the entry to true and return to the message overview. Hit enter to send the command.

If you type e you are going to the next higher level or leave the interactive mode. If you leave the interactive mode you should see the status messages in json format.