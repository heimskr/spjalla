#!/bin/sh

# Inits and updates submodules. Useful for fresh clones.

git submodule init
git submodule update
cd haunted
git submodule init
git submodule update
cd ../pingpong
git submodule init
git submodule update
