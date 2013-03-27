GNOME Shell Search Provider example app
=======================================

"Search providers" are the officially supported integration point for
third party applications in the GNOME search system.

A search provider can conceptually be seen as the way a third party
application presents itself in the context of a GNOME Shell search.

Technically, it is implemented using DBus service activation, and it
works like this:
- every search provider must implement the
org.gnome.Shell.SearchProvider2 DBus interface, which is documented here
[1], drop a keyfile in /usr/share/gnome-shell/search-providers/
specifying the bus name and object path it will use (see
src/search-example-provider.ini for an example), and have a corresponding
service file for activation like any other DBus service (see
src/org.gnome.SearchExample.SearchProvider.service for an example).
The provider must also be associated with the "manifest" of an application
installed on the system, i.e. its desktop file.
- GNOME Shell will read all the keyfiles from the former directory and
keep track of the installed providers. When the user starts a search,
the GetInitialResultSet() method as above will be called on each
provider; DBus activation will automatically start each process if it's
not running already, and ensure only one process per-provider is in use
at any given time.
- information about the results will be exchanged over the wire between
the Shell and the provider processes, and finally when the user selects
a result, ActivateResult() will be called on the corresponding provider,
which can open a window, launch or another process, or do whatever is
necessary for the result to be presented to the user.

The use of a standard DBus interface and separate processes makes it
very easy to implement search providers in any language offering a DBus
binding, avoids any licensing issues due to linking and gives the
maximum freedom to the provider implementation on how to present
activated results, while keeping the overall architecture easy to use.

[1] https://developer.gnome.org/shell/unstable/gdbus-org.gnome.Shell.SearchProvider2.html