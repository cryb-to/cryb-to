#!/bin/sh

istext() {
	local mimetype=$(svn propget svn:mime-type "$1" 2>/dev/null)
	[ -z "$mimetype" ] || expr "$mimetype" : '^text/.' >/dev/null
}

(svn list -R ; svn stat | awk '$1 == "A" { print $2 }') | sort -u |
while read f ; do
	[ -f "$f" ] || continue
	istext "$f" || continue
	case $f in
	*.sh|*.pl)
		svn propset svn:executable \* $f
		;&
	*)
		svn propset eol-style native $f
		svn propset svn:keywords Id $f
		;;
	esac
done
