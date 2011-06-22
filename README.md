Boostcraft (a.k.a. Holework)
============================

Boostcraft is a from-scratch implementation of a Minecraft server in C++, with
an emphasis on performance, low memory usage, and extensibility.

It's not finished yet, but it's getting there!

Architecture
------------

Internally, Boostcraft uses a heavily event-based architecture to facilitate
communication between core modules. A boost::asio::io_service forms the back-
bone of the system, providing for the asynchronous dispatch and handling of
events, in addition to the asynchronous networking facilities which are used to
communicate with clients.

Boostcraft is roughly divided into two main parts: the network layer and the
application layer. The network layer waits for connections from clients, reads
client messages from TCP sockets, parses them according to the Minecraft server
protocol, and finally translates the client packets into events.

The application layer consists of a loosely-coupled set of modules that respond
to events fired by the network layer. Modules can also raise events of their
own, which can be handled by other modules. Events are the primary means of
communication between components of the system. This allows for extremely loose
coupling within Boostcraft's core.

Extensibility
-------------

A fully-featured set of Python bindings for easy plugin writing is planned.
Python scripts will have the ability to extend the server's functionality by
responding to game events, and will also be able to provide replacements for
core modules, such as terrain generation and storage. Because of Boostcraft's
loosely-coupled structure, it is easy to disconnect a built-in module's event
handlers and connect Python callbacks in their place, effectively replacing
the module with an external script.

Planned features
----------------

* Drop-in compatibility with official Minecraft clients and world files
* Greater single-machine scalability than the official server
* Makes breakfast

