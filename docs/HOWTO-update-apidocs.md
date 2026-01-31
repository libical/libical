# HOWTO: Update the API documentation

How to use doxygen to update the libical API documentation at <http://libical.github.io/libical>.

## Preparations

Make sure you have Doxygen installed; else this will fail.

Then checkout the desired branch (master is for unstable).

## Build the documentation

```shell
     # inside the libical top-level
     mkdir build-apidox
     cd build-apidox
     cmake -G Ninja .. && ninja docs
```

Look in doxygen.log for warnings and errors.  and fix them.

Load the ./apidocs/html/index.html into a web browser and review.

## Uploading the new documentation

```shell
     git checkout gh-pages
     cd ..; rm -rf v3/apidocs; cp -dpr build-apidox/apidocs/html v3/apidocs
     git add v3/apidocs #make sure to include any newly generated files
       OR
     cd ..; rm -rf v4/apidocs; cp -dpr build-apidox/apidocs/html v4/apidocs
     git add v4/apidocs #make sure to include any newly generated files
     git commit -m "update apidox" --no-verify . && git push
     rm -rf build-apidox
```

View the updates on <http://libical.github.io/libical/index.html>
(might take a couple minutes to update).

## Finally

Add the generated build-apidox/libical.tag file to the github release
