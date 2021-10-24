# puppet-git-catalog

## Introduction

`puppet-git-catalog` provides `code-id` and `code-content` commands for
Git-managed [static Puppet catalogs][static-catalogs].

Unlike the example `code-content` command provided in Puppet's documentation,
`puppet-git-catalog` supports recursing into submodules, selecting the correct
commit every time it does so.

## Dependencies

* [libgit2][libgit2]

## Building and installation

Run `make` followed by `make install`. The `code-id` and `code-content` commands
are installed in `/usr/local/lib/puppet-git-catalog` by default (this can be
tweaked with the standard `DESTDIR` and `PREFIX` variables).

A drop-in config file is installed in `/etc/puppetlabs/puppetserver/conf.d`,
which points `puppetserver` at the `code-id` and `code-content` commands.

`puppet-git-catalog` assumes your `environmentpath` is set to the default value
(`/etc/puppetlabs/code/environments`).

## Debian package

Pre-built packages for the stable version of Debian are available in my
[personal APT repository][apt-repo].

## License

This project is available under the terms of the ISC license, which is similar
to the 2-clause BSD license. See the `LICENSE` file for the copyright
information and licensing terms.

[apt-repo]: https://www.grahamedgecombe.com/apt-repository
[libgit2]: https://libgit2.org/
[static-catalogs]: https://puppet.com/docs/puppet/7/static-catalogs.html
