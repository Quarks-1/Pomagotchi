#!/bin/bash

# Upload both code and filesystem to ESP32
echo "Uploading code..."
pio run -t upload
if [ $? -eq 0 ]; then
    echo "Code upload successful!"
    echo "Uploading filesystem..."
    pio run -t uploadfs
    if [ $? -eq 0 ]; then
        echo "Filesystem upload successful!"
        echo "All done! Both code and filesystem have been uploaded."
    else
        echo "Error: Filesystem upload failed!"
        exit 1
    fi
else
    echo "Error: Code upload failed!"
    exit 1
fi 