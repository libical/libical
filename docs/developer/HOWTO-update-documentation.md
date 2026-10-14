# HOWTO: Update Online Documentation

How to update and upload the Libical Book and C/C++ API documentation to <https://libical.github.io/libical>.

## Preparations

Make sure you have Doxygen and mdbook installed; else this will fail.

mdbook is a Rust program that you install with `cargo`: i.e.

```shell
     cargo install mdbook --root=/usr/local # install into /usr/local
```

These instructions work for version 4.0 or above only. Please refer to
[HOWTO-update-apidox.md](HOWTO-update-apidox.md) if you need to update
the C/C++ API documentation for version 3.0 (note:the Libical Book is
not supported in version 3.0).

Checkout the branch corresponding to the version you want to update:

```shell
     git checkout 4.0 # for version 4.x
       OR
     git checkout master # for the unstable version
```

## Building the documentation

Use the `documentit.sh` script:

```shell
     # at the top of the libical source directory
     ./scripts/documentit.sh
```

Then add/update to the gh-pages branch:

```shell
     git checkout gh-pages

     rm -rf v3/apidocs; cp -dpr build-docs/apidocs/html v3/apidocs
       OR
     rm -rf v4/apidocs && cp -dpr build-docs/apidocs/html v4/apidocs
     rm -rf book && cp -dpr build-docs/book book

     git add v[34]/apidocs book # insure we include any newly generated files
     git commit -m "update documentation" --no-verify . && git push
     rm -rf build-docs
```

## Finally

Review the updated <https://libical.github.io/libical/index.html>  (might take a couple minutes to update)

At release time, don't forget to add the generated build-docs/libical.tag file to the github release.
