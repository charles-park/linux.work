#!/bin/bash

echo odroid | sudo -S /usr/bin/setterm -blank 0 -powersave off
echo odroid | sudo -S ./NetInfoDisplay

