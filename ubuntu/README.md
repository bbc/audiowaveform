# audiowaveform Packaging for Ubuntu Launchpad

## Install dependencies

```bash
sudo apt-get install devscripts dput cdbs
```

## Publish to Launchpad

Edit package.sh to update the audiowaveform version number and add any new Ubuntu releases to publish to.

Use these commands to publish the source package to Launchpad:

```bash
./package.sh checkout
./package.sh sourcepackage
./package.sh debs
./package.sh publish
```
