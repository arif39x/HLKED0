[READ ME]

This text is a quick introduction to the eEye BootRoot project and the eEye BootRootKit network kernel backdoor.  For much more information, please refer to the slides (eeyebootroot.ppt).


[ENCLOSED FILES]

The eEye BootRootKit NDIS backdoor is a demonstration of boot-time Windows kernel subversion technology.  The assembly source code (ebrk.asm) was written for use with MASM 6.11.  It comes in pre-packaged executable form as a floppy disk image (ebrk.img) and as a CD-ROM ISO-9660 image (ebrk.iso).

Note that the ISO is bare-bones and does not contain a file system, only a boot sector.  If you burn it to disc, it will for the most part appear to be a blank CD.

We've also included the source for a very simple demonstration packet (demrsod2.asm), and a compiled binary file (demrsod2.bin) to be used with netcat ("nc -u").

September 20, 2005:  Scott Tenaglia provided a NASM port of the source, included as "ebrknasm.asm".  Many folks were not too keen on the MASM requirement, so this is a big boon to BootRootKit users at large.  Thanks Scott!

May 9, 2007:  We've included source code for two of our BootRoot-derivative works, SysRq and PiXiE.  Both projects are known to have problems with certain BIOSes, and the PiXiE source code is incomplete (no, we're not going to publish a ready-made virus), but we hope releasing the code will encourage further (better than what we've done) development on the BootRoot concept.

For more information on the development of SysRq, check out the following VICE issues:

http://www.eeye.com/html/resources/newsletters/vice/VI20051005.html
http://www.eeye.com/html/resources/newsletters/vice/VI20051104.html

Slides for the PiXiE presentation are available in the Papers section of research.eeye.com.


[OVERVIEW]

eEye BootRoot is a project presented at Black Hat USA 2005 by Derek Soeder and Ryan Permeh of eEye Digital Security.  The goal was to explore *and* *implement* technology that custom boot sector code could use to subvert the Windows NT-family kernel as it loads.  To our knowledge, such technology had not previously been publicly demonstrated.

eEye BootRootKit is a manifestation of this technology -- a removable-media boot sector that situates itself to regain execution later, as Windows is loading, and then seamlessly continues the boot sequence from hard drive 0.  The basic concept employed is to hook INT 13h and "virtually patch" the Windows OS loader as it's read from disk, then leverage this patch to hook into NDIS.SYS after it has been loaded into memory and validated.

The hook function's purpose is simple: scan all incoming Ethernet frames for a signature in a specific location, and execute code (with kernel privileges) from any matching frame.  The RSoD2 demo gives a very simple display of this capability, by patching NTOSKRNL.EXE in memory and causing a "red screen of death" kernel crash.  Try sending the packet to a closed UDP port on a firewalled machine running BootRootKit, or use the broadcast address!


[FREQUENTLY ASKED QUESTIONS]

Q: Why is it "eEye BootRoot"?

A: Someone else is already using the "BootRoot", so we wanted the distinction to be absolutely clear.


Q: How does it work?

A: Please refer to the slides (eeyebootroot.ppt) included in this package.  They were written to cover every detail and even provide some handy reference material in case you're interested in producing a derivative work, or writing your own.


Q: Is BootRootKit a virus?

A: No, it does not modify the contents of the hard drive, nor any other non-volatile storage.  And before you even ask, it only hooks NDIS.SYS to monitor incoming packets, so no, it does not send any traffic from the system on which it's running.


Q: From what other media could BootRootKit load?

A: Theoretically, any boot media.  We haven't experimented with bootable USB drives, although we do have a working PXE BOOTP/TFTP server for serving up BootRootKit which we're not releasing at this time.  BootRootKit could of course be modified to exist as a replacement hard drive MBR, but again, this would require some code changes.


Q: I attended the presentation and got a CD, but it's empty!  What's the deal?

A: It's not empty, it just doesn't contain any files.  No, seriously.  It has a BootRootKit boot sector, which is of course "below" the ISO-9660 file system, so it wasn't necessary to put any files on the disc.  If you don't believe me, try booting from it and see if the nefarious "blue smiley" appears in the upper-left corner.  Then try sending yourself the demrsod2.bin sample packet with "nc -u", but save your work first.  Or rip the CD back into an ISO file and inspect the contents.

   Oh, and thanks for checking out our talk!  =)


[FEEDBACK]

Please send questions, comments, and anything else eEye BootRoot-related to {dsoeder,rpermeh} at eeye.com.
