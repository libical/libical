# Release Checklist

## Prerelease Source Checking

* [Update zoneinfo](HOWTO-update-zoneinfo.md)
* For minor or patch releases: do an [ABI Compliance Check](HOWTO-abi-changes.md)
* Look for any [Fuzzer problems](https://oss-fuzz.com/testcases?project=libical) we forgot to fix
* Build locally using `./scripts/buildtests.sh` and ensure:
  * no test failures
  * no compile warnings
  * no static checking issues
* Review `CHANGELOG.md` and `README.md`
* Merge any changes made during the review
* Check that the [github CI](https://github.com/libical/libical/actions) is green for the X.Y branch
* Follow the [prerelease source code review](prerelease-source.md) instructions.

## Prerelease Build Checking

Any last changes before pushing the tag? Any CI failures?

If so:

```shell
    git -d vX.Y.Z # delete the tag; good thing we didn't push it yet
    commit and push fixes
    update CHANGELOG.md as necessary
    ./scripts/release-libical.sh X.Y.Z
```

## Release

### Push the tag

```shell
git push --tags
```

### Github

#### Make a release on Github

Go to <https://github.com/libical/libical/releases> and make an official release.

* push the "Draft a new release" button.
* use the CHANGELOG.md to help write the release description.
* hang the libical-X.Y.Z.tar.gz, libical-X.Y.Z.zip
  and libical-X.Y.0-doc.zip (for major and minor releases) on the github release page.

#### Change the default branch

For major (X.Y.0) releases, change the default branch at
<https://github.com/libical/libical/settings> to X.Y

## Postrelease

### Checklist

* [Update the API documentation](HOWTO-update-apidox.md)
* [Update the Book](HOWTO-update-book.md)
* [Ask vcpkg to update the libical port](https://github.com/microsoft/vcpkg/issues/new?assignees=&labels=port+feature&template=request-an-update-to-an-existing-port.md&title=%5B%3Cport+name%3E%5D+update+to+%3Cversion%3E)

### Announcing

Open [Git Discussions](https://github.com/libical/libical/discussions) and make the announcement there.

Here's a sample announcement
>
>Announcing Libical 1.0.1 This is the first bugfix release for Libical 1.0.0
>
>Highlights of this Release:
>
>* highlight1
>* highlight2
>* highlight3
>
>The source code can be found on GitHub at: <https://github.com/libical/libical>
>
>Tarballs and zipballs for v1.0.1 are available from: <https://github.com/libical/libical/releases>
>
>Libical is an Open Source implementation of the iCalendar protocols and protocol data units.
>The iCalendar specification describes how calendar clients can communicate with calendar servers
>so users can store their calendar data and arrange meetings with other users.
>
>For more information about Libical, please visit <http://libical.github.io/libical/>
>

### Prepare for Next Release

* Increase the project() VERSION patch version by 1. ie, X.Y.Z becomes X.Y.Z+1
* Add a new stanza to the top of CHANGELOG.md for "[X.Y.Z+1] - Unreleased"
* Merge to the "master" branch
