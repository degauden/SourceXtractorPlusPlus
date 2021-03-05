# SourceXtractor++
[![Conda](https://img.shields.io/conda/v/astrorama/sourcextractor)](https://anaconda.org/astrorama/sourcextractor)
[![Fedora package](https://img.shields.io/fedora/v/sourcextractor++/rawhide)](https://koji.fedoraproject.org/koji/packageinfo?packageID=30842)
[![Build RPM](https://github.com/astrorama/SourceXtractorPlusPlus/workflows/Build%20RPM/badge.svg)](https://github.com/astrorama/SourceXtractorPlusPlus/actions)
[![Coverity](https://scan.coverity.com/projects/19270/badge.svg?flat=1)](https://scan.coverity.com/projects/astrorama-sextractorxx)
[![Documentation](https://readthedocs.org/projects/sourcextractorplusplus/badge/?version=latest)](https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html)
[![CodeFactor](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/badge/develop)](https://www.codefactor.io/repository/github/astrorama/sourcextractorplusplus/overview/develop)

The next generation SExtractor.

Documentation: https://sourcextractorplusplus.readthedocs.io/en/latest/Introduction.html


[RPMs for Fedora and CentOS are available](https://astrorama.jfrog.io/artifactory/rpm/stable/) You can add a `.repo` file under `/etc/yum.repos.d` with the following content:

```ini
[Artifactory-Astrorama]
name=Artifactory-Astrorama
baseurl=https://astrorama.jfrog.io/artifactory/rpm/stable/fedora/$releasever/$basearch
enabled=1
gpgcheck=0
```

And install normally with

```bash
dnf install SourceXtractorPlusPlus
```

SourceXtractor++ is also available on [Anaconda Cloud for Linux and MacOSX](https://anaconda.org/astrorama/sourcextractor)
```bash
conda install -c conda-forge -c astrorama sourcextractor
 ```
