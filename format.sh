#!/bin/sh

find src -type f -exec clang-format -style=Chromium -i {} \;
