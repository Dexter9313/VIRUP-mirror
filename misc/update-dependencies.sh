#!/bin/bash

libs=$(ldd $1 | awk '/=>/{print $(NF-1)}')
arch=$(dpkg --print-architecture)

packages=""
i=0
N=$(wc -w <<< "$libs")
for lib in $libs; do
	echo -e "\r$((100*i/N))%"
	packs=$(apt-file search $lib | awk '{print $1}' | sed 's/://' | egrep -v "\-dev$" | uniq)
	for pack in $packs; do
		installed=$(dpkg -l | grep $pack)
		if [[ -n $installed ]]; then
			packages="$packages $pack"
		fi
	done
	i=$((1+$i))
done

packages=$(echo $packages | sed 's/ /\n/g' | sort | uniq)

# check if package is a dependency of another
final_packages=""
for p in $packages; do
	rdeps=$(apt-cache rdepends $p)
	keep="true"
	for rd in $packages; do
		if [[ "$p" == "$rd" ]]; then
			continue
		fi
		if [[ -n $(echo $rdeps | sed 's/ /\n/g' | egrep "$rd$") ]]; then
			echo "$rd provides $p"
			keep="false"
			break
		fi
	done
	if [[ "$keep" == "true" ]]; then
		if [[ -z $final_packages ]]; then
			final_packages="$p:$arch"
		else
			final_packages="$final_packages, $p:$arch"
		fi
	fi
done

. /etc/os-release
dep_file=./DEPENDENCIES
echo "$final_packages" > $dep_file
echo "Written in $dep_file"
