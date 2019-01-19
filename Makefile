CFLAGS    ?= -Wall -Wextra -pedantic -O2 -g
PREFIX     = /usr/local
LIBEXECDIR = $(PREFIX)/lib

.PHONY: all clean install uninstall

all: code-content code-id puppet-git-catalog.conf

clean:
	$(RM) code-content code-id puppet-git-catalog.conf

install: all
	install -D code-content $(DESTDIR)$(LIBEXECDIR)/puppet-git-catalog/code-content
	install -D code-id $(DESTDIR)$(LIBEXECDIR)/puppet-git-catalog/code-id
	install -Dm644 puppet-git-catalog.conf $(DESTDIR)/etc/puppetlabs/puppetserver/conf.d/puppet-git-catalog.conf

uninstall:
	$(RM) $(DESTDIR)$(LIBEXECDIR)/puppet-git-catalog/code-content
	$(RM) $(DESTDIR)$(LIBEXECDIR)/puppet-git-catalog/code-id
	$(RM) $(DESTDIR)/etc/puppetlabs/puppetserver/conf.d/puppet-git-catalog.conf

code-content: code-content.c
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c99 -D_GNU_SOURCE -lgit2 -o $@ $<

code-id: code-id.c
	$(CC) $(CFLAGS) $(LDFLAGS) -std=c99 -D_GNU_SOURCE -lgit2 -o $@ $<

puppet-git-catalog.conf: puppet-git-catalog.conf.m4
	m4 -DLIBEXECDIR=$(LIBEXECDIR) $< > $@
