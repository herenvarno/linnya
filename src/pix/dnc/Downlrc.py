#!/usr/bin/env python3
# -*-
import os, re
import urllib, urllib.request, urllib.parse, urllib.error
import sys
import socket
from encodings import gb18030, utf_8, gb2312
from xml.dom import minidom


class LyricsDownloaderQianqian:
	def __init__(self):
		self.title=""
		self.artist=""
		self.dest=""

	def search_and_down(self,title,artist,dest):
		if (title is None )or(dest is None):
			return None
			
		#search and download lrc by title and artist
		#website: qianqian
		title_=self.ToQianQianHexString(title)
		artist_=self.ToQianQianHexString(artist)
		
		URL='http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist=%s&Title=%s&Flags=0' % (artist_, title_)
		#print search information
		print("SEARCH\t[",title,"][",artist,"]");
		
		xmltext = urllib.request.urlopen(URL).read()
		try:
			xmltext = xmltext.decode('gb2312')
			xmltext = xmltext.replace('encoding="gb2312"', 'encoding="utf-8"')
		except:
			pass
		
		xmldoc = minidom.parseString(xmltext)

		ret = []
		root = xmldoc.documentElement

		for node in root.getElementsByTagName('lrc'):
			got_title = node.getAttribute("title").encode("utf-8")
			got_artist = node.getAttribute("artist").encode("utf-8")
			got_id = node.getAttribute("id")
			ret.append({'id': got_id, 'title': got_title, 'artist': got_artist})

		LrcWords=[]
		if len(ret) <= 0:
			print ("FIND\t[Nothing]")
			return None
			
		code = self.CreateQianQianCode(ret[0]['id'], ret[0]['artist'], ret[0]['title'])
		lyrics=urllib.request.urlopen("http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id=%s&Code=%d" % (ret[0]['id'], code)).read().decode("utf_8")
		LrcWords.append(lyrics)
	
		if LrcWords==[]:
			print ("FIND\t[Nothing]")
			return None
		
		fp=open(dest,"w+")
		for s in LrcWords:
			fp.write(s)
		fp.close()
		print ("SAVE\t[%s]" %dest)
		return LrcWords
		
	def QianQianStringFilter(self,string):
		s = string
		# 英文转小写
		s = s.lower()
		# 去括号，大中小还有全角的小括号
		s = re.sub('\(.*?\)|\[.*?]|{.*?}|（.*?）', '', s);
		# 去除半角特殊符号，空格，逗号，etc。
		s = re.sub('[ -/:-@[-`{-~]+', '', s);
		# 去除全角特殊符号
		s = re.sub('[\u2014\u2018\u201c\u2026\u3001\u3002\u300a\u300b\u300e\u300f\u3010\u3011\u30fb\uff01\uff08\uff09\uff0c\uff1a\uff1b\uff1f\uff5e\uffe5]+','', s) 
		return s
	def ToHexStringUnicode(self, string):
		s = string

		tmp = ''
		for c in s:
			dec = ord(c)
			tmp += "%02X" % (dec & 0xff)
			tmp += "%02X" % (dec >> 8)
		return tmp

	def ToHexString(self, string):
		tmp = ''
		for c in string:
			tmp += "%02X" % ord(c)
		return tmp

	def ToQianQianHexString(self, string, RequireUnicode = True):
		if RequireUnicode:
			return self.ToHexStringUnicode(self.QianQianStringFilter(string))
		else:
			return self.ToHexString(string)

	def Conv(self, i):
		r = i % 4294967296
		if (i >= 0 and r > 2147483648):
			r = r - 4294967296
		elif (i < 0 and r < 2147483648):
			r = r + 4294967296
		return r

	def CreateQianQianCode(self, lrcId, artist, title):
		lrcId = int(lrcId)
		ttstr = self.ToQianQianHexString((artist + title).decode("utf-8"), False) ##这里需要utf-8编码
		length = len(ttstr) >> 1
		song = []

		for i in range(length):
			song.append(int(ttstr[i*2:i*2+2], 16))
		t1 = 0
		t2 = 0
		t3 = 0
		t1 = (lrcId & 0x0000FF00) >> 8
		if (lrcId & 0x00FF0000) == 0:
			t3 = 0x000000FF & ~t1
		else:
			t3 = 0x000000FF & ((lrcId & 0x00FF0000) >> 16)

		t3 |= (0x000000FF & lrcId) << 8
		t3 <<= 8
		t3 |= 0x000000FF & t1
		t3 <<= 8

		if (lrcId & 0xFF000000) == 0:
			t3 |= 0x000000FF & (~lrcId)
		else:
			t3 |= 0x000000FF & (lrcId >> 24)

		j = length - 1
		
		while j >= 0:
			c = song[j]
			if c >= 0x80:
				c = c - 0x100
			t1 = (c + t2) & 0x00000000FFFFFFFF
			t2 = (t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF
			t2 = (t1 + t2) & 0x00000000FFFFFFFF
			j -= 1

		j = 0
		t1 = 0

		while j <= length - 1:
			c = song[j]
			if c >= 0x80: # c <128
				c = c - 0x100
			t4 = (c + t1) & 0x00000000FFFFFFFF
			t1 = (t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF
			t1 = (t1 + t4) & 0x00000000FFFFFFFF
			j += 1

		t5 = self.Conv(t2 ^ t3)
		t5 = self.Conv(t5 + (t1 | lrcId))
		t5 = self.Conv(t5 * (t1 | t3))
		t5 = self.Conv(t5 * (t2 ^ lrcId))
		t6 = t5
		if (t6 > 2147483648):
			t5 = t6 - 4294967296
		return t5
	
def download_qianqian_search_and_down(title, artist, dest):
	print("HERE");
	ld=LyricsDownloaderQianqian()
	return ld.search_and_down(title,artist,dest)
	
if __name__ == "__main__":
	download_qianqian_search_and_down("home", "westlife", "/home/yy/a.lrc")
