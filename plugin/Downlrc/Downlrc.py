#!/usr/bin/env python3
# -*-
import os, re
import urllib.request, urllib.parse, urllib.error
import sys
import socket

class wget:
	def __init__(self):
		self.__OPENER = None
		socket.setdefaulttimeout(6) 
		
	def __initOpener(self):
		proxy = os.getenv("http_proxy")
			
		if proxy :
			http_proxy = urllib.ProxyHandler({"http":proxy})
			opener = urllib.request.build_opener(http_proxy)
		else:
			opener = urllib.request.build_opener()
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
		self.title=""
		self.artist=""

	def search(self,title,artist):
		if title is None:
			return[]
				
		#replace spaces in title an artist to ＋
		title=title.replace(" ","+")
		artist=artist.replace(' ','+')
		#search and download lrc by title and artist
		#website: sogou.com
		w=wget()
		URL="http://www.lrc123.com?"
		URL=URL+urllib.parse.urlencode({"keyword" : title , "field":"lyrics"})
		print(URL)
		#print search information
		print("SEARCH\t[",title,"][",artist,"]");
		
		#download webpage
		htmls = w.geturlaslines(URL)
		
		urls={}
		for html in htmls:
			prog = re.compile(r'''<a href="(.*)" class="O3" target="_blank">(.*)LRC歌词下载</a> ''')
			m = prog.search(html.decode("UTF-8"))
			if(m==None):
				continue
			url = m.group(1)
			info = m.group(2)
			urls[url]=info
			
			if(len(urls)>=3):
				break
		return urls
	
	def choose(self,LrcUrls):
		print("abc")
			
	def save(self,LrcUrl,LrcPath):
		w=wget()
		LrcWords=w.geturlaslines(LrcUrl)
		#analyse lrc file
		art=tit=None
		for n in LrcWords:
			n = n.decode("GB18030")
			if re.search("^\[ar:",n): 
				art = n[n.find("[ar:"):]
				art = art[4:art.find("]")]
			if re.search("^\[ti:",n):
				tit = n[n.find("[ti:"):]
				tit = tit[4:tit.find("]")]
			if art!=None and tit!=None:
				break
		if tit!=None and art!=None:
			print("FIND\t[",tit,"][",art,"]-[from lrc123.com]")
			self.__artist = artist
			self.__title = title
		
		#no lyrics, print information and return
		if LrcWords==[]:
			print ("FIND\t[Nothing]-[from lrc123.com]")
			return

		#convert and save lyrics
		fp=open(LrcPath,"w")
		while(LrcWords):
			s=LrcWords.pop()
			fp.write(s.decode("GB18030"))
		fp.close()
		#print information
		print ("SAVE\t[",LrcPath,"]")

def download_lrc123(title, artist):
	print("Download lyrics from lrc123.com, title is", title, ".")
	ld=LyricsDownloader()
	LrcUrls=ld.search(title,artist)
	print(LrcUrls)
	return LrcUrls
	
if __name__=="__main__":
	download_lrc123("我就这样", "westlife")