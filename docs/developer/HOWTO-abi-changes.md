# HOWTO: Keep ABI compatibility between major releases

How to use abi-compliance-checker and abi-dumper to keep ABI compatibility between major releases.

We want to guarantee ABI compliance for all releases with the same major version number.
To do that, we use [abi-dumper](https://github.com/lvc/abi-dumper) and
[abi-compliance-checker](https://github.com/lvc/abi-compliance-checker).

## Preparations

Of course you need a reference version first.  This will be the tag corresponding to the
first ".0.0" release of a new major version.

So until you are preparing a non".0.0" release you can ignore these instructions.
Otherwise:

Make sure you have abi-dumper program which is needed to generate the dump data
for each library release and abi-compliance-checker which compares the dump
data between two library releases.

## Comparing the dump data for a release

First make sure the project version in the top-level CMakeLists.txt is updated for the new release.

And checkout that branch for the new release (should be branch called X.Y).

Then run:

```shell
    ./scripts/cmp-abi-data-libical.sh
```

The standard output will show compatibility and you can look at the web pages reported there as well.
the comparison is stored in the compat_reports directory.
