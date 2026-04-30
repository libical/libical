# Release Checklist

## Prerelease Source Checking

* Write an announcement message for Github discussions and add it to `docs/X.Y.Z/announcement.md`
* Write a release description for Github releases and add it to `docs/X.Y.Z/release.md`
* [Update zoneinfo](HOWTO-update-zoneinfo.md)
* For minor or patch releases: do an [ABI Compliance Check](HOWTO-abi-changes.md)
* Look for any [Fuzzer problems](https://oss-fuzz.com/testcases?project=libical) we forgot to fix
* Build locally using `./scripts/buildtests.sh` and ensure:
  * no test failures
  * no compile warnings
  * no static checking issues
* Review the `README.md`
* Verify the project() `VERSION` and `LIBICAL_LIB_MAJOR_SOVERSION` values in the top-level `CMakeLists.txt`
* Add the real release date (YYYY-MM-DD) to the `CHANGELOG.md` section header "[X.Y.Z] - Unreleased"
* Ensure the Doxygen.cmake file is up-to-date for the doxygen version we use

    ```shell
    doxygen -u  docs/Doxyfile.cmake
    ```

* Run a pre-commit check

    ```shell
    pre-commit run --all-files
    ```

* Merge any changes made during the review
* Check that the [github CI](https://github.com/libical/libical/actions) is green for the X.Y branch

## Prerelease Build Checking

Any last changes before pushing the tag? Any CI failures?

If so:

```shell
    git tag -d vX.Y.Z # delete the tag; good thing we didn't push it yet
    commit and push fixes
    update CHANGELOG.md as necessary
```

## Release

### Create the tag and tar/zipballs

See the bash script `scripts/release-libical.sh` which will prompt to create the tag
and then create  the source tar/zip balls for hanging on the Github release pages.

(NOTE: that this script does **not** push the tag.  Pushing the tag must be done by-hand).

Additionally, `scripts/release-libical.sh` does a build of the documentation
plus creates a zip of the documentation.

To use it run:

```shell
    ./scripts/release-libical.sh X.Y.Z # where X.Y.Z is the release version.
```

### Push the tag

Double-check anything and then when ready => push the tag:

```shell
git push --tags
```

## Github

### Make a release on Github

Go to <https://github.com/libical/libical/releases> and make an official release.

* push the "Draft a new release" button.
* use `docs/X.Y.Z/release.md` as the release description.
* hang the libical-X.Y.Z.tar.gz, libical-X.Y.Z.zip
  and libical-X.Y.0-doc.zip (for major and minor releases) on the github release page.

### Change the default branch

For major (X.Y.0) releases, change the default branch at
<https://github.com/libical/libical/settings> to X.Y

## Postrelease

### Checklist

* [Update the API documentation](HOWTO-update-apidox.md)
* [Update the Book](HOWTO-update-book.md)
* [Ask vcpkg to update the libical port](https://github.com/microsoft/vcpkg/issues/new?assignees=&labels=port+feature&template=request-an-update-to-an-existing-port.md&title=%5B%3Cport+name%3E%5D+update+to+%3Cversion%3E)

### Announcing

Open [Git Discussions](https://github.com/libical/libical/discussions) and make the announcement there.

Paste `docs/X.Y.Z/announcement.md`

### Prepare for Next Release

* Increase the project() VERSION patch version by 1. ie, X.Y.Z becomes X.Y.Z+1
* Add a new stanza to the top of CHANGELOG.md for "[X.Y.Z+1] - Unreleased"
* Merge to the "master" branch

* Review <https://github/libical/libical/issues> for issues that should
  move to a new milestone. Create the new milestone as necessary.
