AM_CPPFLAGS = \
	$(SEARCH_EXAMPLE_CFLAGS) \
	-DDIRECTORY_DIR=\"$(directorydir)\" \
	-DGNOMELOCALEDIR=\"$(datadir)/locale\" \
	$(NULL)

bin_PROGRAMS = gnome-search-example

dbus_search_provider_built_sources = \
	search-example-provider-generated.c \
	search-example-provider-generated.h \
	$(NULL)

$(dbus_search_provider_built_sources) : Makefile.am $(top_srcdir)/src/shell-search-provider-dbus-interfaces.xml
	gdbus-codegen \
		--interface-prefix org.gnome. \
		--c-namespace SearchExample \
		--generate-c-code search-example-provider-generated \
		$(top_srcdir)/src/shell-search-provider-dbus-interfaces.xml \
		$(NULL)

gnome_search_example_LDADD = \
	$(SEARCH_EXAMPLE_LIBS) \
	$(NULL)

gnome_search_example_SOURCES = \
	$(dbus_search_provider_built_sources) \
	gnome-search-example.c \
	$(NULL)

desktopdir = $(datadir)/applications
desktop_in_files = gnome-search-example.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

searchproviderdir = $(datadir)/gnome-shell/search-providers
searchprovider_DATA = search-example-provider.ini

servicedir = $(datadir)/dbus-1/services
service_DATA = $(service_in_files:.service.in=.service)
service_in_files = org.gnome.SearchExample.SearchProvider.service.in

org.gnome.SearchExample.SearchProvider.service: org.gnome.SearchExample.SearchProvider.service.in Makefile
	$(AM_V_GEN) sed -e "s|\@bindir\@|$(bindir)|" $< > $@

CLEANFILES = \
	$(desktop_in_files) \
	$(desktop_DATA) \
	$(NULL)

EXTRA_DIST = \
	gnome-search-example.desktop.in.in \
	$(searchprovider_DATA) \
	$(NULL)
