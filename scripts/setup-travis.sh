#/bin/sh -f

# things to do for travis-ci in the before_install section

if ( test "`uname -s`" = "Darwin" )
then
  brew update
  brew install cmake
else
  #install a newer cmake since at this time Travis only has version 2.8.7
  echo "yes" | sudo add-apt-repository ppa:kalakris/cmake
  sudo apt-get update -qq
  sudo apt-get install cmake
fi
