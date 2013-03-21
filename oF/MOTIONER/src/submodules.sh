#!/bin/bash

# usage: $ ./submodules.sh


if [ -r .git ]; then

	# submodule init & update
	echo "[start submodule init]"
	git submodule init
	echo "end submodule init."

	echo "[start submodule update]"
	git submodule update
	echo "end submodule update."

else

	# clone from repo
	cd ./addons

	echo "[couldn't found ./.gitmodule. start clone five addons from github repos instead of git submodule update]"

	git clone https://github.com/YoshitoONISHI/ofxException.git
	git clone https://github.com/YoshitoONISHI/ofxNonCopyable.git
	git clone https://github.com/YoshitoONISHI/ofxEvent.git
	git clone https://github.com/YoshitoONISHI/ofxSimpleKalmanFilter.git
	git clone https://github.com/YoshitoONISHI/ofxUITabbedCanvas.git
	git clone https://github.com/YoshitoONISHI/ofxGLUtils.git

	echo "end clone addons"
	cd ../
fi
