#!/bin/sh


mntdrvs=`find /Volumes -maxdepth 1`
copydirs=`find . -type d | grep __SCE__`

IFS='
'

for dir in $mntdrvs
do
chkpath="$dir/PSP/GAME"

if [ -d "$chkpath" ]
then
	psp_path="$dir"
	psp_game="$chkpath"
	break
fi
done


if [ ".$psp_path" = "." ]
then
	echo "PSP Not found!!"
	exit 1
else
	echo "Unmount PSP.."
	diskutil unmount $psp_path
fi


