# Pre-release source code review

See the bash script `scripts/release-libical.sh` in this project
which automatically performs all these checks.  Additionally,
`scripts/release-libical.sh` does a build of the documentation
plus creates the tar/zip balls.  If all goes well, the user
will be prompted to create the tag.

To use it run:

```shell
    ./scripts/release-kddockwidgets.sh X.Y.Z` # where X.Y.Z is the release version.
```

## Pre-release checklist

If you would prefer to do some things by hand and bypass the `release-libical.sh`  tool:

* ensure the Doxygen.cmake file is up-to-date for the doxygen version we use

    ```shell
    doxygen -u  docs/Doxyfile.cmake
    ```

* run a pre-commit check

    ```shell
    pre-commit run --all-files
    ```

* verify the project() `VERSION` and `LIBICAL_LIB_MAJOR_SOVERSION` values in the top-level CMakeLists.txt

* Add the real release date (YYYY-MM-DD) to the CHANGELOG.md section header "[X.Y.Z] - Unreleased"

* `git commit` any changes made above and make sure the CIs are green before continuing.

Then create the new tag using the command:

```shell
    git tag -m vX.Y.Z X.Y.Z"
```

**DO NOT PUSH THE TAG YET!**
