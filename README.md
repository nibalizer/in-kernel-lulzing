LOL
===


Character device driver for /dev/lol. It will print out "LOLOLO" when ``cat /dev/lol`` is performed.


Usage
=====


Make sure you have some form of linux headers installed.

  make
  cp 010_lol.rules  /etc/udev/rules.d
  sudo insmod lol.ko


To start every time at boot

  cp lol.ko /lib/modules/`uname -r`
  echo 'lol' >> /etc/modules

You may or may not have to rebuild this module when you next upgrade your kernel. Simply moving it to the new directory might work.
