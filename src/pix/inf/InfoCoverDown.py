#!/usr/bin/env python3
# -*-

import os,sys,re

def cover_search_and_down(artist, album, dest):
	os.system("wget 'http://www.douban.com/subject_search?search_text="+artist.replace(' ','+')+" "+album.replace(' ','+')+"&cat=1003' -O /tmp/linnya/.AlbumCoverPage")

	file=open("/tmp/linnya/.AlbumCoverPage")
	content=file.read()
	file.close()
	os.system("rm /tmp/linnya/.AlbumCoverPage")

	p=re.compile('<a [^>]+>[^<]*<img src="([^"]+)" [^>]*>[^<]*</a>',re.IGNORECASE)
	m=p.search(content)
	if m:
		address=m.group(1)
		address=address.replace('spic', 'lpic')
		os.system("wget '"+address+"' -O '"+ dest + artist + "-"+album+".jpg'")
	else:
		print("[Found Nothing]")

if __name__ == "__main__":
	cover_search_and_down("westlife", "back home", "/home/yy/")
