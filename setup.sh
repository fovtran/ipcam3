#!/bin/bash

python3 -m virtualenv --always-copy --no-site-packages /tmp/pyenv
. /tmp/pyenv/bin/activate

pip3 install pipreqs
# pipreqs /path/to/project
pip3 install -r requirements.txt
