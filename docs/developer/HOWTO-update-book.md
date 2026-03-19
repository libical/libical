# HOWTO: Updating the Book

How to mdbook to update the libical book.

## Preparations

Make sure you have mdbook installed; else this will fail.

mdbook is a Rust program that you install with `cargo`.
ie. `cargo install mdbook --root=/usr/local` # install into /usr/local

Checkout the branch corresponding to the version you want to update:

```shell
     git checkout 3.0 # for version 3.x
       OR
     git checkout master # for version 4.x
```

## Building the book

```shell
     mkdir build-book
     cd build-book
     cmake -G Ninja .. && ninja build-book
     cd ..
```

Then add/update to the gh-pages branch:

```shell
     git checkout gh-pages

     rm -rf book
     cp -r  build-book/book book
     git add book
     git commit -m "Update the Book" ./book --no-verify && git push
     rm -rf build-book
```

## Finally

Review the updated <https://libical.github.io/libical/book> (might take a couple minutes to update)
