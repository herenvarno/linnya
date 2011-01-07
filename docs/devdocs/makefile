STYLESHEETS_DIR = /usr/share/xml/docbook/xsl-stylesheets-1.76.0
INPUT = doc
all: html
html:
	xsltproc $(STYLESHEETS_DIR)/xhtml/chunk.xsl $(INPUT).xml
clean:
	rm -rf *.html
