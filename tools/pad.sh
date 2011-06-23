#!/bin/sh
# Cooltrainer.org kernel padder - 20100704

if [ ! -r kernel.bin ];
then
	echo "kernel.bin not found."
	exit
fi

if ! type perl >/dev/null 2>&1; then
    echo "Perl was not found."
    exit
fi

cp kernel.bin kernel.bin.page

bytes=$(ls -lrt kernel.bin | awk '{print $5}')
fillbytes=$((4096 - ($bytes % 4096)))
echo "Padding kernel.bin to 4096 byte pages as kernel.bin.page"

while [ $fillbytes -gt 0 ]; do
	#printf "\xff" >> only seemed to work in ZSH
	echo "\xff" | perl -ne 's/([0-9a-f]{2})/print chr hex $1/gie' >> kernel.bin.page
	fillbytes=$(($fillbytes - 1))
done

hexcvt ()
{
	if [ -z "$1" ]
	then
		echo 0
		return
	fi

	echo ""$1" "16" o p" | dc
	return
}

paddedbytes=$(ls -lrt kernel.bin.page | awk '{print $5}')

echo -n "Padded kernel size: 0x" && hexcvt $paddedbytes
