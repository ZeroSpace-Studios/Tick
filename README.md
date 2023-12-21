## Tick

Tick is an open protcol with the aim of providing several benefits over LTC. Based off of high-precision network timing protocols it aims to provide a simple way to drive timing parameters across devices.


## Installation

To use simply include the pre-built library in your project. Currently only a C++ library is provided, but a C library is planned in the very near future.

## Usage

To use the library simply include the header file and link the library.

```cpp
// To setup a tick receiver

#include "tick.h"

// Create a tick receiver
TickReceiver receiver;

// Setup the receiver to listen.

receiver.setup("127.0.0.1");

// To setup a tick sender

TickSender sender;

// Setup the sender to send to the receiver.

sender.setup("ip of the reciver");

// To send a tick, provide a time in a UINT64 formatted microsecond timestamp.

sender.sendTime(0);

//To receive a tick, call the receive function.

uint64_t time = receiver.receiveTime();

```