assisi-arena-ui
===============

Bee arena user interface for the ASSISI|bf project.

Instructions for users
----------------------

Add the ASSISI ppa, update and install:

```
sudo add-apt-repository ppa:damjan-miklic-larics/assisi
sudo apt-get update
sudo apt-get install assisi-arena-ui
```

Developer instructions
----------------------

Instructions for developers.

Prerequisites
~~~~~~~~~~~~~

qt5-dev

Building the code
~~~~~~~~~~~~~~~~~

From Qt Creator 

Debian packaging
----------------

Instructions for making a Debian package have been taken from here: https://bhavyanshu.me/how-to-make-debian-packages-for-qt-c-based-applications/11/10/2014

Environment setup
~~~~~~~~~~~~~~~~~

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

Building the package
~~~~~~~~~~~~~~~~~~~~

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
lintian ../assisi-arena-ui_x.y.z.-?ubuntu?_source.changes
```

Sign the source package:
```
debsign ../assisi-arna-ui_x.y.z.-?ubuntu?_source.changes
```

Uploading to the PPA
~~~~~~~~~~~~~~~~~~~~

Official Launchpad instructions are here: https://developer.ubuntu.com/en/publish/other-forms-of-submitting-apps/ppa/

```
dput ppa:damjan-miklic-larics/assisi assisi-arena-ui_x.y.z-1ubuntu_source.changes
```
