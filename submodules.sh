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
	cd ./oF/MOTIONER/src/addons

	echo "[couldn't found ./.gitmodule. start clone seven addons from github repos instead of git submodule update]"

	git clone https://github.com/rezaali/ofxUI.git
	cd ofxUI
	git checkout 93715f1
	cd ../

	git clone https://github.com/YoshitoONISHI/ofxException.git
	cd ofxException
	git checkout de4507b
	cd ../

	git clone https://github.com/YoshitoONISHI/ofxNonCopyable.git
	cd ofxNonCopyable
	git checkout ffa3e53
	cd ../

	git clone https://github.com/YoshitoONISHI/ofxEvent.git
	cd ofxEvent
	git checkout 3cacf82
	cd ../

	git clone https://github.com/YoshitoONISHI/ofxUITabbedCanvas.git
	cd ofxUITabbedCanvas
	git checkout 89ec49e
	cd ../

	echo "end clone addons"
	cd ../../../../
fi
