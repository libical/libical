# HOWTO: Update Zoneinfo

How to use the vzic program to update the libical builtin zoneinfo database.

## Preparations

1. Retrieve the most recent tzdata from IANA from <http://www.iana.org/time-zones>

   Look for a gzip-compressed tar file (a "tarball") named tzdata2025c.tar.gz, for example.

   Save the tarball into your Downloads folder.

2. Uncompress and untar the "tarball" file into the vzic folder:

     ```shell
     cd ./vzic
     mkdir tzdata2025c
     cd tzdata2025c; tar xvfz ~/Downloads/tzdata2025c.tar.gz; cd ..
     ```

## Building vzic

```shell
     cd ./vzic
     make clean
     make -B CREATE_SYMLINK=0 IGNORE_TOP_LEVEL_LINK=0
     # pass PRODUCT_ID and TZID_PREFIX as necessary (shouldn't be) or you could edit the top-level Makefile
```

## Running vzic

```shell
     cd ./vzic
     ./vzic --olson-dir=tzdata2024b
```

The output is placed in the zoneinfo subdirectory by default, but you can use the --output-dir options
to set another toplevel output directory.

You'll probably see some warning messages but not sure what to do about those.

The "Modifying RRULE to be compatible with Outlook" warnings are normal and can be ignored.

## Merging Changes to the Master Set of VTIMEZONES

Now we have a new set of zoneinfo in the vzic folder. We need to merge
those changes into the official zoneinfo one level-up.

1. Run

     ```shell
     cd ./vzic
     ./vzic-merge.pl --master-zoneinfo-dir=../zoneinfo --new-zoneinfo-dir=./zoneinfo
     ```

2. Add the new timezones in the zones.tab file by hand

     run `git status` # look for new .ics files to be added (i.e untracked files in the zoneinfo directory)

3. diff the new ./vzic/zoneinfo/zones.tab versus the updated zones.tab in ../zoneinfo/zones.tab

     copy lines from the new vzic/zoneinfo/zones.tab that are new (or differ) into the official ../zoneinfo/zones.tab

## Testing

1. run `make test-vzic` and then run `./test-vzic`  (make sure libical is installed to /usr/local)
2. run the libical test-suite (`./scripts/buildtests.sh`)

## Clean-up

You can remove:

* the tzdata tarball (eg. ~/Downloads/tzdata2024b.tar.gz)
* the exploded tzdata (eg. ./vzic/tzdata2024b)
* the new zoneinfo (./vzic/zoneinfo)
