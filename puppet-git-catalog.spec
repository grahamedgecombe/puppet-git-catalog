Name:    puppet-git-catalog
Version: 1.0.0
Release: 1%{?dist}
Summary: Submodule-aware Git-managed static Puppet catalogs

License: ISC
URL:     https://github.com/grahamedgecombe/puppet-git-catalog
Source0: https://github.com/grahamedgecombe/puppet-git-catalog/archive/v%{version}.tar.gz

BuildRequires: libgit2-devel
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
* Sat Jan 19 2019 Graham Edgecombe <gpe@grahamedgecombe.com> - 1.0.0-1
- Initial release
