# HOWTO: Update Zoneinfo

How to use the vzic program to update the libical builtin zoneinfo database.

Most of this work is automated now by `./scripts/update-zoneinfo.sh`.
Please use that script (update as needed).

## Preparations

1. Retrieve the most recent tzdata from IANA from <https://www.iana.org/time-zones>

   Look for a gzip-compressed tar file (a "tarball") named tzdata2026a.tar.gz, for example.

   Save the tarball into your Downloads folder.

2. Uncompress and untar the "tarball" file into the vzic folder:

     ```shell
     cd ./vzic
     mkdir tzdata2026a
     cd tzdata2026a; tar xfz ~/Downloads/tzdata2026a.tar.gz; cd ..
     ```

## Building vzic

```shell
     # inside the vzic subdir
     mkdir -p build-update-zoneinfo
     cd build-update-zoneinfo
     cmake -DSTANDALONE=True ..
     # pass -DPRODUCT_ID=<id> and -DTZID_PREFIX=<prefix> as necessary (shouldn't be)
     cmake --build .
```

## Running vzic

```shell
     # inside the vzic/build-update-zoneinfo subdir
     ./vzic --pure --olson-dir ../tzdata2026a --output-dir ./zoneinfo
```

You'll might see some warning messages but not sure what to do about those.

The "Modifying RRULE to be compatible with Outlook" warnings are normal and can be ignored.

## Merging Changes to the Master Set of VTIMEZONES

Now we have a new set of zoneinfo in the vzic folder. We need to merge
those changes into the official zoneinfo one level-up.

1. Run

     ```shell
     # inside the vzic subdir
     perl vzic-merge.pl --master-zoneinfo-dir=../zoneinfo --new-zoneinfo-dir=build-update-zoneinfo/zoneinfo
     ```

2. Add the new timezones in the zones.tab file by hand

     run `git status` # look for new .ics files to be added (i.e untracked files in the zoneinfo directory)

3. Merge changes in the new zones.tab to the one in the libical/zoneinfo

     ```shell
     # inside the vzic subdir
     diff ./build-update-zoneinfo/zoneinfo/zones.tab  ../zoneinfo/zones.tab
     ```

     copy lines from the generated zones.tab that are new (or differ) into the official zones.tab.

## Testing

1. run the libical test-suite (`./scripts/buildtests.sh`)

## Last Steps

1. Update the CHANGELOG.md accordingly

2. Commit the changes

     ```shell
     git switch -c work/winterz/tzdata2026a #make the changes in a work branch
     git add zoneinfo; git commit zoneinfo CHANGELOG.md -m "Update zoneinfo for tzdata2026a"
     ```

3. Make a pull request with the changes and if all goes well go ahead and merge.

## Clean-up

You can remove:

* the tzdata tarball (eg. ~/Downloads/tzdata2026a.tar.gz)
* the exploded tzdata (eg. vzic/tzdata2026a)
* the vzic builddir (vzic/build-update-zoneinfo)
* the work branch work/winterz/tzdata2026a
