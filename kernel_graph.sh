#!/usr/bin/bash

lsmod |
perl -e 'print "digraph \"lsmod\" {";
         <>;
         while(<>) {
            @_ = split/\s+/;
            print "\"$_[0]\" -> \"$_\"\n"
            for split/,/,$_[3];
         }
         print "}";' |
dot -Tpdf > a.pdf

