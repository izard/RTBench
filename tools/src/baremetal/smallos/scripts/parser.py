#!/usr/bin/python
#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# 
#  Copyright (c) 2001-2009, Intel Corporation
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
# 
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
# 
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
#   3. Neither the name of the Intel Corporation nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
 
import os, sys
import re
import getopt

VERSION=0.15

class SegmentError(Exception):
    def __init__(self,value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class Segmant(object):
    name     = ""
    elf_type = ""
    phyAddr  = 0
    offset   = 0
    size     = 0
    es       = ""
    flg      = ""
    lk       = 0
    inf      = 0
    al       = 0
    hexdata  = list()
    header   = list()
    def __init__(self,args):
        self.name     = args[0]
        self.elf_type = args[1]
        self.phyAddr  = int(args[2],16)
        self.offset   = int(args[3],16)
        self.size     = int(args[4],16)
        self.es       = args[5]
        self.flg      = args[6]
        self.lk       = int(args[7],10)
        self.inf      = int(args[8],10)
        self.al       = int(args[9],10)

    def isAlloc(self):
        return ('A' in self.flg)

    def isMerge(self):
        return ('M' in self.flg)

    def __str__(self):
        return self.name

    def getHeader(self,kernel='kernel'):
        data = []
        if(len(self.header) == 0):
            buf = self.getBin(kernel)
            lines = len(buf)
            if(lines <= 256):
                #self.header.append('/origin %08x\n' % self.phyAddr)
                data.append('/origin %08x\n' % self.phyAddr)
            else:
                count = 0
                #round up
                #final = lines/256 + (0 if (lines%256 == 0) else 1)
                final = lines/256 + ({True: 0, False: 1}[lines%256 == 0])
                while(count < final):
                    #self.header.append('/origin %08x\n' % \
                    #        (self.phyAddr+count*4096))
                    data.append('/origin %08x\n' % (self.phyAddr+count*4096))
                    count += 1

            self.header = data
        return self.header

    def getBin(self,kernel='kernel'):
        if(not self.isAlloc()):
            raise SegmentError("Can't generate bin data on non-allocated sect.")

        if(len(self.hexdata) == 0):
            cmd = 'hexdump -v -s %d -n %d' % (self.offset,self.size)
            cmd += ' -e \'16/1 "%02X " "\n"\''
            cmd += ' ' + kernel
            f = os.popen(cmd)
            self.hexdata = f.readlines()
            f.close()

        return self.hexdata

class binary(object):
    def combine(self,msegs):
        args = (
        ','.join([seg.name for seg in msegs]),
        msegs[0].elf_type,
        str(hex(msegs[0].phyAddr)),
        str(hex(msegs[0].offset)),
        str(hex(sum(seg.size for seg in msegs))),
        msegs[0].es,
        msegs[0].flg.replace('M',''),
        str(msegs[0].lk),
        str(msegs[0].inf),
        str(msegs[0].al)
        )

        return Segmant(args)

    def process(self):
        merge = []
        for s in self.segs:
            try:
                if(s.isAlloc()):
                    if(s.isMerge()):
                        merge.append(s)
                    else:
                        if(len(merge) != 0):
                            self.toFile(self.combine(merge))
                            merge = []
                        self.toFile(s)
            except SegmentError, e:
                #print >> self.log_stream, "SegmangError:%s" % (str(e))
                pass

    def toFile(self,seg):
        header = seg.getHeader()
        print >> self.log_stream, seg.name + str(header)
        data = seg.getBin()
        count = 0
        out = open(self.outName,'a')
        if(len(header) > 1):
            for l in header[:-1]:
                out.write(l)
                for ll in data[count*256:(count+1)*256]:
                    out.write(ll)
                count += 1
        out.write(header[-1])
        for ll in data[count*256:]:
            out.write(ll)
        out.close()

    def __init__(self,inF='kernel',outF='linux_boot/smallOS.bin',log=sys.stdout):
        self.log_stream = log
        print >> self.log_stream, "LWRTE Parser %1.2f, inFile:%s, outFile:%s" % (VERSION,inF, outF)
        self.segs = list()
        self.inName = inF
        self.outName = outF

    def clean(self):
        if(os.path.exists(self.outName)):
            print >> self.log_stream, "Cleaning old file:%s" % (self.outName)
            os.remove(self.outName)

    def build(self,buf):
        for l in buf:
            try:
                buf=re.split('\s+',l.split('] ')[1])[:-1]
                if(len(buf)==10):
                    self.segs.append(Segmant(buf))
            except IndexError, e:
                #print >> self.log_stream, "IndexError:%s" % (str(e))
                pass
            except ValueError, e:
                #print >> self.log_stream, "ValueError:%s" % (str(e))
                pass

    def start(self):
        buf=os.popen('readelf -S -W %s' % (self.inName)).readlines()
        self.clean()
        self.build(buf)
        self.process()


if __name__ == "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:], "i:o:", ["input=","output="])
        outFile = None
        inFile = None

        for o, a in opts:
            if (o in ("-i", "--input")):
                inFile = a
            elif(o in ("-o", "--output")):
                outFile = a
            else:
                assert False, "unhandled option"

        if(not None in (outFile, inFile)):
            log_stream = sys.stdout
            #log_stream = open('log.txt','w')
            binary(inFile,outFile,log_stream).start()
            log_stream.close()
    except getopt.GetoptError, err:
        print str(err)
        sys.exit(2)
    


