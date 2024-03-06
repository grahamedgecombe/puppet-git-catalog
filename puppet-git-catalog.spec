Name:    puppet-git-catalog
Version: 2.0.0
Release: 1%{?dist}
Summary: Submodule-aware Git-managed static Puppet catalogs

License: ISC
URL:     https://github.com/grahamedgecombe/puppet-git-catalog
Source0: https://github.com/grahamedgecombe/puppet-git-catalog/archive/v%{version}.tar.gz

BuildRequires: libgit2-devel >= 1.0.0
BuildRequires: m4

%description
Provides code-id and code-content commands for Git-managed static Puppet
catalogs.

Unlike the example code-content command provided in Puppet's documentation,
puppet-git-catalog supports recursing into submodules, selecting the correct
commit every time it does so.

%prep
%setup -q -n %{name}-%{version}

%build
%make_build DESTDIR=%{buildroot} LIBEXECDIR=%{_libexecdir}

%install
%make_install DESTDIR=%{buildroot} LIBEXECDIR=%{_libexecdir}
install -Dm644 LICENSE %{buildroot}/%{_defaultlicensedir}/%{name}-%{version}/LICENSE

%files
%{_libexecdir}/puppet-git-catalog/code-content
%{_libexecdir}/puppet-git-catalog/code-id
%config(noreplace) %{_sysconfdir}/puppetlabs/puppetserver/conf.d/puppet-git-catalog.conf
%{_defaultlicensedir}/%{name}-%{version}/LICENSE

%changelog
* Wed Mar 06 2024 Graham Edgecombe <gpe@grahamedgecombe.com> - 2.0.0-1
- Fix compatibility with v1 of libgit2

* Tue Mar 12 2019 Graham Edgecombe <gpe@grahamedgecombe.com> - 1.0.2-1
- Fix reading an empty file with code-content

* Thu Feb 21 2019 Graham Edgecombe <gpe@grahamedgecombe.com> - 1.0.1-1
- Check for errors when writing to stdout

* Sat Jan 19 2019 Graham Edgecombe <gpe@grahamedgecombe.com> - 1.0.0-1
- Initial release
