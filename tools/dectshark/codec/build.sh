gcc -g -o decode-g72x decode.c g*.c bitstream.c
echo "This code is released under the GNU GPL unless stated otherwise within. Public domain code by SUN Microsystems is used. There's also GNU LGPL code from the spandsp project."
echo 'Sample usage: for i in *.ima ; do cat $i | decode-g72x -64 -l -R | sox -r 8000 -2 -c 1 -s -t raw - -t wav $i.wav; done'
