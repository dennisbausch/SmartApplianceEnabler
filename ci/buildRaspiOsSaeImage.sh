#!/bin/sh
#
# Copyright (C) 2021 Axel Müller <axel.mueller@avanux.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
IMAGE_URL=https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2020-12-04/2020-12-02-raspios-buster-armhf-lite.zip
MOUNT_POINT=mnt
COMPRESSE_IMAGE_FILE=raspios-buster-armhf-lite.zip

echo "Downloading Raspbian OS image from $IMAGE_URL"
curl -o $COMPRESSE_IMAGE_FILE $IMAGE_URL

unzip $COMPRESSE_IMAGE_FILE
IMAGE_FILE=`find . -name *.img`
echo "Image file is $IMAGE_FILE"

PARTITION_OFFSET=`fdisk -l $IMAGE_FILE | grep 83 | awk '{print $2}'`
echo "Linux partition offset is $PARTITION_OFFSET"

echo "Creating mount point "
mkdir $MOUNT_POINT
sudo mount -t auto -o loop,offset=$((PARTITION_OFFSET * 512)) $IMAGE_FILE $MOUNT_POINT
ls -al $MOUNT_POINT