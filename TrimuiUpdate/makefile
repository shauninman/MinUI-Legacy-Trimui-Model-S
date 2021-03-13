# TrimuiUpdate

.PHONY: clean

all:
	cd ./launcher && zip TrimuiUpdate_MinUI.zip updater
	cp ./launcher/TrimuiUpdate_MinUI.zip ./installer
	cp ./launcher/updater ./installer/launcher.sh

clean:
	rm -f ./launcher/TrimuiUpdate_MinUI.zip
	rm -f ./installer/TrimuiUpdate_MinUI.zip
	rm -f ./installer/launcher.sh