FILES = download.html faq.html features.html howto.html index.html screenshots.html status.html vision.html xpertmud-design.html tutorial.html

download.html = "Xpertmud - Download"
faq.html = "Xpertmud - FAQ"
features.html = "Xpertmud - Features"
howto.html = "Xpertmud - HOWTO"
index.html = "Xpertmud - Linux KDE and Windows MUD Client"
screenshots.html = "Xpertmud - Screenshots"
status.html = "Xpertmud - Status"
vision.html = "Xpertmud - Vision"
xpertmud-design.html = "Xpertmud - Design"
tutorial.html = "Xpertmud - Plugin Tutorial"

all: $(FILES)

%.html: %-m.html frame-upper frame-lower
	python generate.py $@ frame-upper frame-lower $($@)


