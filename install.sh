#!/bin/bash
sudo cp `dirname $0`/libdatetime1.so /usr/lib/dde-dock/plugins/libdatetime1.so
killall dde-dock