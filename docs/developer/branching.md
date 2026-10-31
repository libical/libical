# Branching

Stable branches are branched from the "master" branch and called X.Y,
where 'X' the major version number and 'Y' is the minor version number.

We may refer to the "master" branch as the unstable branch.
The currently actively X.Y branch is known as the stable branch.

Bugfixes can stay in the current stable branch and do not require a new branch, which
means that the following instructions can be skipped for patch-point releases.

## New minor version

For small feature modifications we need to create a new stable branch.
If the current stable branch is X.Y then the new branch will be called X.Y+1.

For example, if the current stable branch is 2.1 then the new stable branch will be 2.2, like so:

```shell
    git checkout master
    git checkout -b 2.2
```

## New major version

However, for large feature modifications or API changes we need a major stable release.
If the current stable branch is X.Y then the new branch will be called X+1.0.

For example, if the current stable branch is 2.1 then the new stable branch will be 3.0, like so:

```shell
    git checkout master
    git checkout -b 3.0
```

## After branching

Once the new branch is created we need to modify a few things.

### Source code changes

For a new major version X.0.0:

* In the top-level `CMakeLists.txt` set the project() `VERSION` to X-1.99.99
* In the top-level `CMakeLists.txt` set `LIBICAL_LIB_MAJOR_SOVERSION` to the new major version X
* In the top-level `CMakeLists.txt` set the `LIBICAL_LIB_MINOR_SOVERSION` to 0
* Change the `PROJECT_VERSION` to X.0
* In the `CHANGELOG.md` add a empty section at the top for the new "[X.0.0] - Unreleased"

  For example, if the new major version is 5.0.0, then:

  * The project() `VERSION` becomes 4.99.99
  * The `LIBICAL_LIB_MAJOR_SOVERSION` becomes 5
  * The `LIBICAL_LIB_MINOR_SOVERSION` becomes 0
  * The `PROJECT_VERSION` becomes 5.0

For a new minor version X.Y.0:

* In the top-level `CMakeLists.txt` set the project() `VERSION` to X.Y.0
* Do NOT change the `LIBICAL_LIB_MAJOR_SOVERSION`
* Increment the `LIBICAL_LIB_MINOR_SOVERSION` minor value to Y
* Change the `PROJECT_VERSION` to X.Y
* In the `CHANGELOG.md` add a empty section at the top for the new "[X.Y.0] - Unreleased"

  For example, if the new minor version is 4.2.0, then:

  * The project() `VERSION` becomes 4.2.0
  * The `LIBICAL_LIB_MAJOR_SOVERSION` is still 4
  * The `LIBICAL_LIB_MINOR_SOVERSION` becomes 2
  * The `PROJECT_VERSION` becomes 4.2

Make all the above changes and then git commit and push the new branch.

### And finally

* Update the Github Default Branch (see Default Branch setting at <https://github.com/libical/libical/settings>)
* Initiate github CI builds of the new branch and make sure all is green before continuing.
