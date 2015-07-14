SOX=/usr/bin/sox
 
for i in `/bin/ls -1 *.pcap` ; do
        ./pcapstein $i
done
 
#decoder for g.721
#        for i in *.ima ; do
#                cat $i | ./decode-g72x -4 -a | sox -r 8000 -1 -c 1 -A -t raw - -t wav $i.g721.wav;
#        done
 
#decoder for g.726.R
        for i in *.ima ; do
                cat $i | ./decode-g72x -64 -l -R | sox -r 8000 -2 -c 1 -s -t raw - -t wav $i.g726.R.wav;
        done
 
#decoder for g.726.L
#        for i in *.ima ; do
#                cat $i | ./decode-g72x -64 -l -L | sox -r 8000 -2 -c 1 -s -t raw - -t wav $i.g726.L.wav;
#        done

rm *.ima
