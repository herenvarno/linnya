#!/usr/bin/env python
# -*- coding: utf-8 -*-


# if you use python2 use #!/usr/bin/env python2 to replace the first line.


# NAME:	LyricsDownloader.py
# DISCRIPTION:
#	Download Lyrics from Web
#	This file can only provide a basic function to download lrc file from Internet.
#	There is not any garantee of accuracy.
#	However, this is an extensible file, If you are a professor of python, try to modify it as you like
# PARAMETER:
#	path:	the path of lrc file to be saved
#	title:	the title of music to be searched for
#	artist:	the artist of music to be searched for
#

import os, re
import urllib2, dbus
import sys

class wget:
	def __init__(self):
		self.__OPENER = None
		
	def __initOpener(self):
		proxy = os.getenv("http_proxy")
			
		if proxy :
			http_proxy = urllib2.ProxyHandler({"http":proxy})
			opener = urllib2.build_opener(http_proxy)
		else:
			opener = urllib2.build_opener()
					
		self.__OPENER = opener
					
	def geturlasfile(self,url):
		if self.__OPENER is None :
			self.__initOpener()
							
		rlfile = self.__OPENER.open(url)
		return urlfile
							
	def geturlaslines(self,url):
		if self.__OPENER is None :
			self.__initOpener()
									
		urlfile = self.__OPENER.open(url)
		return urlfile.readlines()
class LyricsDownloader:
	def __init__(self):
		''' initial function lrc_search '''
		self.title=""
		self.artist=""
		
	def search(self,title,artist="unknown"):
		''' search lrc from web '''
		
		#title is none, return
		if title is None:
			return[]
		
		#replace spaces in title an artist to ＋
		title=title.replace(" ","+")
		artist=artist.replace(' ','+')
		
		#search and download lrc by title and artist
		#website: sogou.com
		w=wget()
		URL="http://mp3.sogou.com/gecisearch.so?query=%s+%s" % (artist,title)
		
		#print search information
		print "SEARCH\t[%s][%s]" %(title,artist)
		
		#download webpage
		URL=URL.encode("GBK")
		urllib2.quote(URL)
		htmls = w.geturlaslines(URL)
		
		#analyse webpage, find the url link and download the lrc file
		for html in htmls:
			if html.find("downlrc.jsp")<0:
				continue
			link = html[html.find("downlrc.jsp"):html.find("uigs")-2]
			link = "http://mp3.sogou.com/" + link
			LrcWords=w.geturlaslines(link)
			
			#analyse lrc file
			art=tit=None
			for n in LrcWords:
				n = unicode(n,"GB18030").encode("UTF8")
				if re.search("^\[ar:",n): 
					art = n[n.find("[ar:"):]
					art = art[4:art.find("]")]
				if re.search("^\[ti:",n):
					tit = n[n.find("[ti:"):]
					tit = tit[4:tit.find("]")]
				if art!=None and tit!=None:
					break
			if tit!=None and art!=None:
				print "FIND\t[%s][%s]-[from Sogou.com]" % (tit,art)
				self.__artist = artist
				self.__title = title
				return LrcWords
		return []
	
	def save(self,LrcWords,LrcPath):
		''' save lrc '''
		
		#no lyrics, print information and return
		if LrcWords==[]:
			print "FIND\t[Nothing]-[from Sogou.com]"
			return
		
		#convert and save lyrics
		s = "".join(LrcWords)
		s = unicode(s,"GB18030").encode("UTF8")
		
		fp=open(LrcPath,"w+")
		fp.write(s)
		fp.close()
		
		#print information
		print "SAVE\t[%s]" %LrcPath

if __name__ == "__main__":
	argc = len(sys.argv)
	ld=LyricsDownloader();
	
	if argc==3:
		LrcWords=ld.search(unicode(sys.argv[2],"UTF-8"))
		ld.save(LrcWords,unicode(sys.argv[1],"UTF-8"))
	elif argc==4:
		LrcWords=ld.search(unicode(sys.argv[2],"UTF-8"),unicode(sys.argv[3],"UTF-8"))
		ld.save(LrcWords,unicode(sys.argv[1],"UTF-8"))
