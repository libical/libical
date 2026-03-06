# HOWTO: Update C/C++ API Documentation

How to use doxygen to update and upload the C/C++ API documentation to <http://libical.github.io/libical>.

## Preparations

Make sure you have Doxygen installed; else this will fail.

Checkout the branch corresponding to the version you want to update:

```shell
     git checkout 3.0 # for version 3.x
       OR
     git checkout master # for version 4.x
```

## Building the documentation

```shell
     mkdir build-apidox
     cd build-apidox
     cmake -G Ninja .. && ninja docs
     cd ..
```

Then add/update to the gh-pages branch:

```shell
     git checkout gh-pages

     rm -rf v3/apidocs; cp -dpr build-apidox/apidocs/html v3/apidocs
       OR
     rm -rf v4/apidocs; cp -dpr build-apidox/apidocs/html v4/apidocs

     git add v[34]/apidocs # insure we include any newly generated files
     git commit -m "update apidox" --no-verify . && git push
     rm -rf build-apidox
```

## Finally

Review the updated <http://libical.github.io/libical/index.html>  (might take a couple minutes to update)

At release time, don't forget to add the generated build-apidox/libical.tag file to the github release.
