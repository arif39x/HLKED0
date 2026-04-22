Atomic2	v1.1
============

- What does Atomic2 ?

Atomic2 is primarily a Dial Up password retriever, but
it can be modified to do all sorts of things.

- How it works ?

Once Atomic2 executes it copyies itself to "dialupsc.exe"
in your windows system directory and creates a registry
entry under "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\
Windows\CurrentVersion\Run" named "DialUpSecurity". Then
waits for a Dial Up connection to be made and when the
connection is alive it sends all the information via a
SMTP server to one ore more email adresses. After that
it saves all information that was sent to a file in the
system directory ("rasxnfo.dll") and closes itself.
When restarting windows it will close automaticly if the
new information match the information that was already
sent. The data is encoded (use "decode.exe" to decode it,
input file: encode_in.txt , output file: decode_out.txt)

- That's all ?

;-) that's all. The main feature of this program is it's
size (36KB) and because of this it can be easily emailed
or transfered to any computer. Also, as I said before, if
you want to add more capabilities it's easy.

For more information you'll have to read the program
sources included in this package.

Atomic2 should work on all windows platforms (including
NT & W2K). It was tested on 98 & NT 4.0. The project is
designed for VC++ 6.0.

(c) 2000-2001 Marius David
