#/bin/sh -f

# things to do for travis-ci in the before_install section

if ( test "`uname -s`" = "Darwin" )
then
  brew install libxml2
  brew install icu4c
  brew install db
  brew install gobject-introspection
  brew install gtk-doc
else
  sudo apt-get update -qq
  sudo apt-get install libicu-dev
  sudo apt-get install libdb4.8-dev
  sudo apt-get install gobject-introspection libgirepository1.0-dev
  sudo apt-get install gtk-doc-tools
fi
