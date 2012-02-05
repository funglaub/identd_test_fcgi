FastCGI app written in C to query your identd service
=====================================================

Motivation
----------

I became tired of using `telnet` and `netstat` to test my identd service so I decided to write a small Webapp in C with [FastCGI](http://www.fastcgi.com/drupal/) to achieve this. This programm uses Unix sockets to connect to your identd daemon and prints its response unformatted.

Compilation & Running
---------------------

### Compilation

To compile the small program simply use `make`:

   make

You might want to adjust the `CC` line in the Makefile depending where your `fcgi` includes an libs reside.

### Launching
Launch the program with 

       spawn-fcgi -p<PORT> ./ident_query

where PORT corresponds to the port in your webserver configuration. You can also use Unix domain sockets, for further instruction refer to `spwan-fcgi(1)`.

Demo
----
http://ident.roladder.net