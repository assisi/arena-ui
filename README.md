# assisi-arena-ui #

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1323953.svg)](https://doi.org/10.5281/zenodo.1323953)

Bee arena user interface for the ASSISI|bf project.

## Instructions for users ##


Add the ASSISI ppa, update and install:

```
sudo add-apt-repository ppa:damjan-miklic-larics/assisi
sudo apt-get update
sudo apt-get install assisi-arena-ui
```

## Developer instructions ##

The current officially supported development platform is Ubuntu 16.04 Xenial Xerus. 

### Prerequisites ###


```
qt5-dev
libzmq-dev
libzmqpp-dev
libyaml-cpp-dev
libprotobuf-dev
libboost-dev
```

### Building the code ###

From Qt Creator ...

### Building the docs

User documentation can be compiled in the doc/user directory:

```
make html
make latexpdf
```

## Debian packaging ##


Instructions for making a Debian package have been taken from [here](https://bhavyanshu.me/how-to-make-debian-packages-for-qt-c-based-applications/11/10/2014).

General instructions for using the Pbuilder packaging tool on Ubuntu are available [here](https://wiki.ubuntu.com/PbuilderHowto).

### Environment setup ###


Install the necessary tools:

```
sudo apt-get install pdebuild
```

Create a `.pbuilderrc` file with the following contents:

```
BASETGZ=$HOME/pbuilder/base.tgz
BUILDPLACE=$HOME/pbuilder/build/
BUILDRESULT=$HOME/pbuilder/result/
DEBEMAIL='Your Name <your@email.domain>'
BUILDUSERNAME=username
AUTO_DEBSIGN=yes
APTCACHEHARDLINK=no
DEBBUILDOPTS="-sa"
COMPONENTS="main restricted universe multiverse"
```

If you're short on disk space on the partition where $HOME is installed, you can put the pbuilder folder on another disk/partition and adjust the paths in `.pbuilderrc` accordingly.

Create the pbuilder environment:

```
sudo pbuilder --create
```

After making any changes to `.pbuilderrc` you might need to `sudo pbuilder update --override-config`

### Building the package ###

Make sure you have updated the `debian/changelog` file appropriately.

Switch to the folder where you have cloned the git repo, and export the source. All changes need to be committed! Pay special attention to `-` and `_` signs, they are different in different commands. This is a bit convoluted but supposedly necessary. (TODO: try using git-buildpackage, it should be simpler).

```
mkdir -p ../debs/assisi-arena-ui-x.y.z
git archive master | tar -x -C ../debs/assisi-arena-ui-x.y.z
cd ../debs/assisi-arena-ui-x.y.z
tar cvf - * | gzip > ../assisi-arena-ui_x.y.z.orig.tar.gz
dh_make -e your@email.domain -f ../assisi-arena-ui_x.y.z.orig.tar.gz -s -c lgpl3
pdebuild --debbuildopts -sa
```

It's good form to run a lintian check on the generated source package:

```
lintian ../assisi-arena-ui_x.y.z.-0ubuntu1~xenial1_source.changes
```

Sign the source package:
```
debsign ../assisi-arena-ui_x.y.z.-0ubuntu1~xenial1_source.changes
```

### Uploading to the PPA ###

Official Launchpad instructions are here: https://developer.ubuntu.com/en/publish/other-forms-of-submitting-apps/ppa/

```
dput ppa:damjan-miklic-larics/assisi ../assisi-arena-ui_x.y.z-0ubuntu1~xenial1_source.changes
```

Getting the `Successfully uploaded packages.` message does not necessarily mean that your upload was successful :) You should get an e-mail confirmation of the successful upload. If the e-mail states that your upload has been rejected try to fix the issue, delete the `assisi-arena-ui_x.y.z-?ubuntu?_source.ppa.upload` file and re-upload. Repeat as necessary :)