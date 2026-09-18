#!/bin/sh
set -eu

mkdir -p /run/sshd
ssh-keygen -A
/usr/sbin/sshd

exec runuser -u user -- "$@"
