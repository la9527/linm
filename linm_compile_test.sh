#!/bin/bash

LINM_INSTALL_PATH=$(pwd)/output/install
CONFIG_PATH=$(pwd)/sh

$LINM_INSTALL_PATH/bin/linm --cfg=$CONFIG_PATH/default.cfg  --col=$CONFIG_PATH/colorset.cfg  --key=$CONFIG_PATH/keyset.cfg $1
