#!/bin/bash

# things to do for travis-ci in the before_install section

# Install a supported cmake version (>= 3.11.0)
wget -O cmake.sh https://cmake.org/files/v3.11/cmake-3.11.0-Linux-x86_64.sh 
sudo sh cmake.sh --skip-license --exclude-subdir --prefix=/usr/local

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  brew install libxml2
  brew install icu4c
  brew install db
  brew install gobject-introspection
  brew install gtk-doc
else
  sudo apt-get update -qq
  sudo apt-get install libicu-dev
  sudo apt-get install libdb-dev
  sudo apt-get install gobject-introspection libgirepository1.0-dev
  sudo apt-get install gtk-doc-tools
fi
