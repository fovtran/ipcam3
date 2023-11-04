#!/bin/bash

python3 -m virtualenv --always-copy --clear /tmp/pyenv && export STAGE=1
. /tmp/pyenv/bin/activate

pip3 install pipreqs
pip3 install -r requirements.txt && export STAGE=1

exit 0