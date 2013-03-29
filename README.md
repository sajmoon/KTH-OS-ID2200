KTH-OS-ID2200
=============

KTH course about OS.

Lab 1.
Make a program that pipes data from printenv | sort | less

where less could change to whatever is specified in the PAGER envvariable.

If the program is started with arguments then those should be piped through grep like such:

printenv | grep -i arguments | sort | less
